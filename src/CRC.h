#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// ========================================================================
// Reason to live:	CRC (Cyclic Redundancy Code) implementation using
//					template metaprogramming to create the lookup table.
//					Used to illustrate a real-life usage of TMP
//
// Notes:			Might use a lot of complier heap memory. If using
//					VC the compiler option /Zm400 can be used to increase
//					memory allocation limit (400% worked for me)
//					
// ========================================================================

// ========================================================================
// CRC definitions:
// CRC_NAME		- common name of algorithm specification (optional)
// CRC_WIDTH	- number of bits in resulting hash
// CRC_POLY		- the (truncated) polynomial used (e.g. 0x8005 = x^16+x^3+1)
// CRC_INIT		- initial remainder 
// CRC_REFIN	- reflect data?
// CRC_REFOUT	- reflect remainder?
// CRC_XOROUT	- final XOR value
// CRC_CHECKSTRING - a string to calculate CRC on... (optional)
// CRC_CHECK	- ... and the resulting CRC value
// ========================================================================
/*
#define   CRC_NAME		  "CRC-32"
#define   CRC_WIDTH		  32
#define   CRC_POLY		  0x04C11DB7
#define   CRC_INIT		  0xFFFFFFFF
#define   CRC_REFIN		  TRUE
#define   CRC_REFOUT	  TRUE
#define   CRC_XOROUT	  0xFFFFFFFF
#define   CRC_CHECKSTRING "123456789"
#define   CRC_CHECK		  0xCBF43926
*/

#define   CRC_NAME		  "CRC16-ARC"
#define   CRC_WIDTH		  16
#define   CRC_POLY		  0x8005
#define   CRC_INIT		  0x0000
#define   CRC_REFIN		  TRUE
#define   CRC_REFOUT	  TRUE
#define   CRC_XOROUT	  0000
#define   CRC_CHECKSTRING "123456789"
#define   CRC_CHECK		  0xBB3D

/*
#define   CRC_NAME		  "CRC16-CCITT"
#define   CRC_WIDTH		  16
#define   CRC_POLY		  0x1021
#define   CRC_INIT		  0xFFFF
#define   CRC_REFIN		  FALSE
#define   CRC_REFOUT	  FALSE
#define   CRC_XOROUT	  0x0000
#define   CRC_CHECKSTRING "123456789"
#define   CRC_CHECK		  0x29B1
*/
// ========================================================================
// MACRO definitions
// ========================================================================
// mask bit X
#define	  BITMASK(X) (1L << (X))
// mask bits 0-X
#define   BITMASKTO(X) ~( (-1) << (X) )
// simplify code
#define   BYTE_SHIFT (CRC_WIDTH - 8)
#define   TOPBIT BITMASK(CRC_WIDTH-1)

// FIRST/FINAL used to create table (to simplify code when reflection is used)
#if TRUE == CRC_REFIN
#define FIRST(i) (REFLECT<(i),8>::RESULT)
#define FINAL(i) (REFLECT<(i),CRC_WIDTH>::RESULT)
#else 
#define FIRST(i) (i)
#define FINAL(i) (i)
#endif // CRC_REFIN

// HASH_TYPE is the value returned from the hash function
#if 32 == CRC_WIDTH
#define HASH_TYPE unsigned int
#else
#if 16 == CRC_WIDTH
#define HASH_TYPE unsigned short
#else 
#define HASH_TYPE unsigned char
#endif
#endif

#ifdef _DEBUG
#define PRINT printf
#if 32 == CRC_WIDTH
#define PRINT_V(i) printf("%08X",i)
#else
#if 16 == CRC_WIDTH
#define PRINT_V(i) printf("%04X",i)
#else 
#define PRINT_V(i) printf("%02X",i)
#endif
#endif
#endif // _DEBUG

// ========================================================================
// Template classes for creating the table 
// ========================================================================
namespace REFLECT_INTERNAL
{
 template<unsigned int W, unsigned int b>
 struct REFLECTED_PART{
	// unroll loop
	template<unsigned int i>
	struct REF
	{
		enum 
		{RESULT = ( (0 == (BITMASK(i)&W) ) ? 0 : BITMASK(b-i-1) ) | (REF<i-1>::RESULT)
		};
	};
	// unroll "special case"
	struct REF<0>
	{
		enum 
		{RESULT = ( (0 == (BITMASK(0)&W) ) ? 0 : BITMASK(b-1) )
		};
	};

	public:
	enum
	{RESULT = REF<b>::RESULT
	};
 };
};

// REFLECT::RESULT keeps the value of W with its lowest b bits reflected
template<unsigned int W, unsigned int b>
class REFLECT
{
public:
 enum 
	{RESULT = (W & ~BITMASKTO(b)) | (BITMASKTO(b) & REFLECT_INTERNAL::REFLECTED_PART<W,b>::RESULT)
	};
};

// TABLE_ENTRY::RESULT keeps the lookup table entry at index
template<unsigned int index>
class TABLE_ENTRY
{public:
	// Unroll loop
	enum
	{R0 = ( TOPBIT & (FIRST(index) << (CRC_WIDTH-8)) ) ? (FIRST(index) << ( CRC_WIDTH-7)) ^ CRC_POLY : (FIRST(index) << (CRC_WIDTH-7) ),
	 R1 = ( TOPBIT & R0 ) ? (R0<<1) ^ CRC_POLY : (R0<<1),
	 R2 = ( TOPBIT & R1 ) ? (R1<<1) ^ CRC_POLY : (R1<<1),
	 R3 = ( TOPBIT & R2 ) ? (R2<<1) ^ CRC_POLY : (R2<<1),
	 R4 = ( TOPBIT & R3 ) ? (R3<<1) ^ CRC_POLY : (R3<<1),
	 R5 = ( TOPBIT & R4 ) ? (R4<<1) ^ CRC_POLY : (R4<<1),
	 R6 = ( TOPBIT & R5 ) ? (R5<<1) ^ CRC_POLY : (R5<<1),
	 R7 = ( TOPBIT & R6 ) ? (R6<<1) ^ CRC_POLY : (R6<<1),
	};

public:
	enum
	{RESULT = FINAL(R7) & BITMASKTO(CRC_WIDTH)
	};
};


// ========================================================================
// Class Definition
// ========================================================================

class CRC 
{
public:
	static inline HASH_TYPE Hash(unsigned char* pBuffer, unsigned int nBufferSize);
	static inline unsigned int HashSize() {return CRC_WIDTH;} 
#ifdef _DEBUG
	static void print_table();
#endif

private:
	static HASH_TYPE m_crctable[256];
};


// ========================================================================
// Class Implementation
// ========================================================================

// Initiated by compiler
HASH_TYPE CRC::m_crctable[256] = {
TABLE_ENTRY<0>::RESULT,   TABLE_ENTRY<1>::RESULT,   TABLE_ENTRY<2>::RESULT,   TABLE_ENTRY<3>::RESULT,
TABLE_ENTRY<4>::RESULT,   TABLE_ENTRY<5>::RESULT,   TABLE_ENTRY<6>::RESULT,   TABLE_ENTRY<7>::RESULT,
TABLE_ENTRY<8>::RESULT,   TABLE_ENTRY<9>::RESULT,   TABLE_ENTRY<10>::RESULT,  TABLE_ENTRY<11>::RESULT,
TABLE_ENTRY<12>::RESULT,  TABLE_ENTRY<13>::RESULT,  TABLE_ENTRY<14>::RESULT,  TABLE_ENTRY<15>::RESULT,
TABLE_ENTRY<16>::RESULT,  TABLE_ENTRY<17>::RESULT,  TABLE_ENTRY<18>::RESULT,  TABLE_ENTRY<19>::RESULT,
TABLE_ENTRY<20>::RESULT,  TABLE_ENTRY<21>::RESULT,  TABLE_ENTRY<22>::RESULT,  TABLE_ENTRY<23>::RESULT,
TABLE_ENTRY<24>::RESULT,  TABLE_ENTRY<25>::RESULT,  TABLE_ENTRY<26>::RESULT,  TABLE_ENTRY<27>::RESULT,
TABLE_ENTRY<28>::RESULT,  TABLE_ENTRY<29>::RESULT,  TABLE_ENTRY<30>::RESULT,  TABLE_ENTRY<31>::RESULT,
TABLE_ENTRY<32>::RESULT,  TABLE_ENTRY<33>::RESULT,  TABLE_ENTRY<34>::RESULT,  TABLE_ENTRY<35>::RESULT,
TABLE_ENTRY<36>::RESULT,  TABLE_ENTRY<37>::RESULT,  TABLE_ENTRY<38>::RESULT,  TABLE_ENTRY<39>::RESULT,
TABLE_ENTRY<40>::RESULT,  TABLE_ENTRY<41>::RESULT,  TABLE_ENTRY<42>::RESULT,  TABLE_ENTRY<43>::RESULT,
TABLE_ENTRY<44>::RESULT,  TABLE_ENTRY<45>::RESULT,  TABLE_ENTRY<46>::RESULT,  TABLE_ENTRY<47>::RESULT,
TABLE_ENTRY<48>::RESULT,  TABLE_ENTRY<49>::RESULT,  TABLE_ENTRY<50>::RESULT,  TABLE_ENTRY<51>::RESULT,
TABLE_ENTRY<52>::RESULT,  TABLE_ENTRY<53>::RESULT,  TABLE_ENTRY<54>::RESULT,  TABLE_ENTRY<55>::RESULT,
TABLE_ENTRY<56>::RESULT,  TABLE_ENTRY<57>::RESULT,  TABLE_ENTRY<58>::RESULT,  TABLE_ENTRY<59>::RESULT,
TABLE_ENTRY<60>::RESULT,  TABLE_ENTRY<61>::RESULT,  TABLE_ENTRY<62>::RESULT,  TABLE_ENTRY<63>::RESULT,
TABLE_ENTRY<64>::RESULT,  TABLE_ENTRY<65>::RESULT,  TABLE_ENTRY<66>::RESULT,  TABLE_ENTRY<67>::RESULT,
TABLE_ENTRY<68>::RESULT,  TABLE_ENTRY<69>::RESULT,  TABLE_ENTRY<70>::RESULT,  TABLE_ENTRY<71>::RESULT,
TABLE_ENTRY<72>::RESULT,  TABLE_ENTRY<73>::RESULT,  TABLE_ENTRY<74>::RESULT,  TABLE_ENTRY<75>::RESULT,
TABLE_ENTRY<76>::RESULT,  TABLE_ENTRY<77>::RESULT,  TABLE_ENTRY<78>::RESULT,  TABLE_ENTRY<79>::RESULT,
TABLE_ENTRY<80>::RESULT,  TABLE_ENTRY<81>::RESULT,  TABLE_ENTRY<82>::RESULT,  TABLE_ENTRY<83>::RESULT,
TABLE_ENTRY<84>::RESULT,  TABLE_ENTRY<85>::RESULT,  TABLE_ENTRY<86>::RESULT,  TABLE_ENTRY<87>::RESULT,
TABLE_ENTRY<88>::RESULT,  TABLE_ENTRY<89>::RESULT,  TABLE_ENTRY<90>::RESULT,  TABLE_ENTRY<91>::RESULT,
TABLE_ENTRY<92>::RESULT,  TABLE_ENTRY<93>::RESULT,  TABLE_ENTRY<94>::RESULT,  TABLE_ENTRY<95>::RESULT,
TABLE_ENTRY<96>::RESULT,  TABLE_ENTRY<97>::RESULT,  TABLE_ENTRY<98>::RESULT,  TABLE_ENTRY<99>::RESULT,
TABLE_ENTRY<100>::RESULT, TABLE_ENTRY<101>::RESULT, TABLE_ENTRY<102>::RESULT, TABLE_ENTRY<103>::RESULT,
TABLE_ENTRY<104>::RESULT, TABLE_ENTRY<105>::RESULT, TABLE_ENTRY<106>::RESULT, TABLE_ENTRY<107>::RESULT,
TABLE_ENTRY<108>::RESULT, TABLE_ENTRY<109>::RESULT, TABLE_ENTRY<110>::RESULT, TABLE_ENTRY<111>::RESULT,
TABLE_ENTRY<112>::RESULT, TABLE_ENTRY<113>::RESULT, TABLE_ENTRY<114>::RESULT, TABLE_ENTRY<115>::RESULT,
TABLE_ENTRY<116>::RESULT, TABLE_ENTRY<117>::RESULT, TABLE_ENTRY<118>::RESULT, TABLE_ENTRY<119>::RESULT,
TABLE_ENTRY<120>::RESULT, TABLE_ENTRY<121>::RESULT, TABLE_ENTRY<122>::RESULT, TABLE_ENTRY<123>::RESULT,
TABLE_ENTRY<124>::RESULT, TABLE_ENTRY<125>::RESULT, TABLE_ENTRY<126>::RESULT, TABLE_ENTRY<127>::RESULT,
TABLE_ENTRY<128>::RESULT, TABLE_ENTRY<129>::RESULT, TABLE_ENTRY<130>::RESULT, TABLE_ENTRY<131>::RESULT,
TABLE_ENTRY<132>::RESULT, TABLE_ENTRY<133>::RESULT, TABLE_ENTRY<134>::RESULT, TABLE_ENTRY<135>::RESULT,
TABLE_ENTRY<136>::RESULT, TABLE_ENTRY<137>::RESULT, TABLE_ENTRY<138>::RESULT, TABLE_ENTRY<139>::RESULT,
TABLE_ENTRY<140>::RESULT, TABLE_ENTRY<141>::RESULT, TABLE_ENTRY<142>::RESULT, TABLE_ENTRY<143>::RESULT,
TABLE_ENTRY<144>::RESULT, TABLE_ENTRY<145>::RESULT, TABLE_ENTRY<146>::RESULT, TABLE_ENTRY<147>::RESULT,
TABLE_ENTRY<148>::RESULT, TABLE_ENTRY<149>::RESULT, TABLE_ENTRY<150>::RESULT, TABLE_ENTRY<151>::RESULT,
TABLE_ENTRY<152>::RESULT, TABLE_ENTRY<153>::RESULT, TABLE_ENTRY<154>::RESULT, TABLE_ENTRY<155>::RESULT,
TABLE_ENTRY<156>::RESULT, TABLE_ENTRY<157>::RESULT, TABLE_ENTRY<158>::RESULT, TABLE_ENTRY<159>::RESULT,
TABLE_ENTRY<160>::RESULT, TABLE_ENTRY<161>::RESULT, TABLE_ENTRY<162>::RESULT, TABLE_ENTRY<163>::RESULT,
TABLE_ENTRY<164>::RESULT, TABLE_ENTRY<165>::RESULT, TABLE_ENTRY<166>::RESULT, TABLE_ENTRY<167>::RESULT,
TABLE_ENTRY<168>::RESULT, TABLE_ENTRY<169>::RESULT, TABLE_ENTRY<170>::RESULT, TABLE_ENTRY<171>::RESULT,
TABLE_ENTRY<172>::RESULT, TABLE_ENTRY<173>::RESULT, TABLE_ENTRY<174>::RESULT, TABLE_ENTRY<175>::RESULT,
TABLE_ENTRY<176>::RESULT, TABLE_ENTRY<177>::RESULT, TABLE_ENTRY<178>::RESULT, TABLE_ENTRY<179>::RESULT,
TABLE_ENTRY<180>::RESULT, TABLE_ENTRY<181>::RESULT, TABLE_ENTRY<182>::RESULT, TABLE_ENTRY<183>::RESULT,
TABLE_ENTRY<184>::RESULT, TABLE_ENTRY<185>::RESULT, TABLE_ENTRY<186>::RESULT, TABLE_ENTRY<187>::RESULT,
TABLE_ENTRY<188>::RESULT, TABLE_ENTRY<189>::RESULT, TABLE_ENTRY<190>::RESULT, TABLE_ENTRY<191>::RESULT,
TABLE_ENTRY<192>::RESULT, TABLE_ENTRY<193>::RESULT, TABLE_ENTRY<194>::RESULT, TABLE_ENTRY<195>::RESULT,
TABLE_ENTRY<196>::RESULT, TABLE_ENTRY<197>::RESULT, TABLE_ENTRY<198>::RESULT, TABLE_ENTRY<199>::RESULT,
TABLE_ENTRY<200>::RESULT, TABLE_ENTRY<201>::RESULT, TABLE_ENTRY<202>::RESULT, TABLE_ENTRY<203>::RESULT,
TABLE_ENTRY<204>::RESULT, TABLE_ENTRY<205>::RESULT, TABLE_ENTRY<206>::RESULT, TABLE_ENTRY<207>::RESULT,
TABLE_ENTRY<208>::RESULT, TABLE_ENTRY<209>::RESULT, TABLE_ENTRY<210>::RESULT, TABLE_ENTRY<211>::RESULT,
TABLE_ENTRY<212>::RESULT, TABLE_ENTRY<213>::RESULT, TABLE_ENTRY<214>::RESULT, TABLE_ENTRY<215>::RESULT,
TABLE_ENTRY<216>::RESULT, TABLE_ENTRY<217>::RESULT, TABLE_ENTRY<218>::RESULT, TABLE_ENTRY<219>::RESULT,
TABLE_ENTRY<220>::RESULT, TABLE_ENTRY<221>::RESULT, TABLE_ENTRY<222>::RESULT, TABLE_ENTRY<223>::RESULT,
TABLE_ENTRY<224>::RESULT, TABLE_ENTRY<225>::RESULT, TABLE_ENTRY<226>::RESULT, TABLE_ENTRY<227>::RESULT,
TABLE_ENTRY<228>::RESULT, TABLE_ENTRY<229>::RESULT, TABLE_ENTRY<230>::RESULT, TABLE_ENTRY<231>::RESULT,
TABLE_ENTRY<232>::RESULT, TABLE_ENTRY<233>::RESULT, TABLE_ENTRY<234>::RESULT, TABLE_ENTRY<235>::RESULT,
TABLE_ENTRY<236>::RESULT, TABLE_ENTRY<237>::RESULT, TABLE_ENTRY<238>::RESULT, TABLE_ENTRY<239>::RESULT,
TABLE_ENTRY<240>::RESULT, TABLE_ENTRY<241>::RESULT, TABLE_ENTRY<242>::RESULT, TABLE_ENTRY<243>::RESULT,
TABLE_ENTRY<244>::RESULT, TABLE_ENTRY<245>::RESULT, TABLE_ENTRY<246>::RESULT, TABLE_ENTRY<247>::RESULT,
TABLE_ENTRY<248>::RESULT, TABLE_ENTRY<249>::RESULT, TABLE_ENTRY<250>::RESULT, TABLE_ENTRY<251>::RESULT,
TABLE_ENTRY<252>::RESULT, TABLE_ENTRY<253>::RESULT, TABLE_ENTRY<254>::RESULT, TABLE_ENTRY<255>::RESULT
};
// ========================================================================

// there are 2x2 variations on the line after the while-loop, depending on how reflection is defined..
HASH_TYPE CRC::Hash(unsigned char* pBuffer, unsigned int nBufferSize)
{
 HASH_TYPE crc = CRC_INIT;
 while(nBufferSize--)
#if TRUE == CRC_REFIN
		 crc = m_crctable[(crc ^ *pBuffer++) & 0xffL] ^ 
#else
		 crc = m_crctable[((crc >> BYTE_SHIFT) ^ *pBuffer++) & 0xffL] ^ 
#endif
#if TRUE == CRC_REFOUT
																		(crc >> 8);
#else
																		(crc << 8);
#endif

	 crc = crc ^ CRC_XOROUT;
	 return crc;

}




// =================================DEBUG=================================
#ifdef _DEBUG

void CRC::print_table()
{int linebreak = 192/CRC_WIDTH;
 for (int i = 0; i<256; i++)
	{PRINT_V(m_crctable[i]);
	 if ( 0 == (i+1)%linebreak || i==255)
		PRINT("\n");
	 else
		PRINT(", ");
	}
}

#endif
