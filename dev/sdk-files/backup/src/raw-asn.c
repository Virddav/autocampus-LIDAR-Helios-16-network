/* Automatically generated file - do not edit */

#include "asn1defs.h"
#include "raw-asn.h"

const ASN1CType asn1_type_RAW_DUMMYPDU[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x4000000 | 0x10,
  2,
  sizeof(RAW_DUMMYPDU),

  offsetof(RAW_DUMMYPDU, header) | 0x0,
  (intptr_t)asn1_type_RAW_ItsPduHeader,
  0,
  (intptr_t)"header",

  offsetof(RAW_DUMMYPDU, dummy) | 0x0,
  (intptr_t)asn1_type_RAW_DummyMsg,
  0,
  (intptr_t)"dummy",

  (intptr_t)"DUMMYPDU",
};

const ASN1CType asn1_type_RAW__local_0[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100000,
  0x0,
  0xff,
};

const ASN1CType asn1_type_RAW__local_1[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x1800000 | 0x100001,
  0x0,
  0xff,
};

const ASN1CType asn1_type_RAW_ItsPduHeader[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x4000000 | 0x100000,
  3,
  sizeof(RAW_ItsPduHeader),

  offsetof(RAW_ItsPduHeader, protocolVersion) | 0x0,
  (intptr_t)asn1_type_RAW__local_0,
  0,
  (intptr_t)"protocolVersion",

  offsetof(RAW_ItsPduHeader, messageID) | 0x0,
  (intptr_t)asn1_type_RAW__local_1,
  0,
  (intptr_t)"messageID",

  offsetof(RAW_ItsPduHeader, stationID) | 0x0,
  (intptr_t)asn1_type_RAW_StationID,
  0,
  (intptr_t)"stationID",

  (intptr_t)"ItsPduHeader",
};

const ASN1CType asn1_type_RAW_StationID[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x5800000 | 0x100002,
  0x0,
  0xffffffff,
  (intptr_t)"StationID",
};

const ASN1CType asn1_type_RAW_DummyMsg[] = {
  (ASN1_CTYPE_SEQUENCE << ASN1_CTYPE_SHIFT) | 0x4000000 | 0x100001,
  2,
  sizeof(RAW_DummyMsg),

  offsetof(RAW_DummyMsg, sequenceNumber) | 0x0,
  (intptr_t)asn1_type_RAW_SequenceNumber,
  0,
  (intptr_t)"sequenceNumber",

  offsetof(RAW_DummyMsg, someData) | 0x0,
  (intptr_t)asn1_type_RAW_SomeData,
  0,
  (intptr_t)"someData",

  (intptr_t)"DummyMsg",
};

const ASN1CType asn1_type_RAW_SequenceNumber[] = {
  (ASN1_CTYPE_INTEGER << ASN1_CTYPE_SHIFT) | 0x5800000 | 0x100000,
  0x0,
  0xffff,
  (intptr_t)"SequenceNumber",
};

const ASN1CType asn1_type_RAW_SomeData[] = {
  (ASN1_CTYPE_OCTET_STRING << ASN1_CTYPE_SHIFT) | 0x5800000 | 0x100001,
  0x1,
  0x14,
  (intptr_t)"SomeData",
};

