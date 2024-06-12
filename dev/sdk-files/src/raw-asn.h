/* Automatically generated file - do not edit */
#ifndef _FFASN1_RAW_ASN_H
#define _FFASN1_RAW_ASN_H

#include "asn1defs.h"

#ifdef  __cplusplus
extern "C" {
#endif

typedef unsigned int RAW_StationID;

extern const ASN1CType asn1_type_RAW_StationID[];

typedef struct RAW_ItsPduHeader {
  int protocolVersion;
  int messageID;
  RAW_StationID stationID;
} RAW_ItsPduHeader;


extern const ASN1CType asn1_type_RAW_ItsPduHeader[];

typedef int RAW_SequenceNumber;

extern const ASN1CType asn1_type_RAW_SequenceNumber[];

typedef ASN1String RAW_SomeData;

extern const ASN1CType asn1_type_RAW_SomeData[];

typedef struct RAW_DummyMsg {
  RAW_SequenceNumber sequenceNumber;
  RAW_SomeData someData;
} RAW_DummyMsg;


extern const ASN1CType asn1_type_RAW_DummyMsg[];

typedef struct RAW_DUMMYPDU {
  RAW_ItsPduHeader header;
  RAW_DummyMsg dummy;
} RAW_DUMMYPDU;


extern const ASN1CType asn1_type_RAW_DUMMYPDU[];

#ifdef  __cplusplus
}
#endif

#endif /* _FFASN1_RAW_ASN_H */
