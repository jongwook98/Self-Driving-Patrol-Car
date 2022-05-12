/* API for S-Functions and Stateflow to support dynamic matrix data */
/* Copyright 2019 The MathWorks, Inc. */

#pragma once

#include "simulink_spec.h"
#include "simstruct/simstruc.h"

SIMULINK_EXPORT_EXTERN_C DTypeId ssRegisterDynamicMatrixDataType(SimStruct* S,
                                                                 DTypeId containedDataTypeId,
                                                                 size_t numDims);

SIMULINK_EXPORT_EXTERN_C bool ssIsDynamicMatrixDataType(SimStruct* S, DTypeId dataTypeId);

SIMULINK_EXPORT_EXTERN_C void ssConstructDynamicMatrix(SimStruct* S,
                                                       DTypeId dataTypeId,
                                                       void* ioBufferSlot);

SIMULINK_EXPORT_EXTERN_C void ssDestructDynamicMatrix(SimStruct* S,
                                                      DTypeId dataTypeId,
                                                      void* ioBufferSlot);

SIMULINK_EXPORT_EXTERN_C void ssConstructDynamicMatrixData(SimStruct* S,
                                                           DTypeId dataTypeId,
                                                           size_t numDims,
                                                           size_t* dims,
                                                           size_t elementSize,
                                                           const void* inData,
                                                           void* ioBufferSlot);

SIMULINK_EXPORT_EXTERN_C size_t ssGetInputPortDynamicMatrixDataNumDims(SimStruct* S, int portIdx);
SIMULINK_EXPORT_EXTERN_C size_t ssGetOutputPortDynamicMatrixDataNumDims(SimStruct* S, int portIdx);

SIMULINK_EXPORT_EXTERN_C void ssGetInputPortDynamicMatrixDataDims(SimStruct* S,
                                                                  int portIdx,
                                                                  size_t* dims);
SIMULINK_EXPORT_EXTERN_C void ssGetOutputPortDynamicMatrixDataDims(SimStruct* S,
                                                                   int portIdx,
                                                                   
                                                                   size_t* dims);

SIMULINK_EXPORT_EXTERN_C size_t ssGetInputPortDynamicMatrixDataWidth(SimStruct* S, int portIdx);
SIMULINK_EXPORT_EXTERN_C size_t ssGetOutputPortDynamicMatrixDataWidth(SimStruct* S, int portIdx);

SIMULINK_EXPORT_EXTERN_C void ssSetOutputPortDynamicMatrixDataDims(SimStruct* S,
                                                                   int portIdx,
                                                                   size_t numDims,
                                                                   const size_t* dims);

SIMULINK_EXPORT_EXTERN_C const void* ssGetInputPortDynamicMatrixData(SimStruct* S, int portIdx);
SIMULINK_EXPORT_EXTERN_C void* ssGetOutputPortDynamicMatrixData(SimStruct* S, int portIdx);

/* eof */
