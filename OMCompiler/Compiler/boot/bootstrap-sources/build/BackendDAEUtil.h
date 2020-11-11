#ifndef BackendDAEUtil__H
#define BackendDAEUtil__H
#include "meta/meta_modelica.h"
#include "util/modelica.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#ifdef __cplusplus
extern "C" {
#endif
DLLExport
modelica_metatype omc_BackendDAEUtil_getAllVarLst(threadData_t *threadData, modelica_metatype _dae);
#define boxptr_BackendDAEUtil_getAllVarLst omc_BackendDAEUtil_getAllVarLst
static const MMC_DEFSTRUCTLIT(boxvar_lit_BackendDAEUtil_getAllVarLst,2,0) {(void*) boxptr_BackendDAEUtil_getAllVarLst,0}};
#define boxvar_BackendDAEUtil_getAllVarLst MMC_REFSTRUCTLIT(boxvar_lit_BackendDAEUtil_getAllVarLst)
DLLExport
modelica_metatype omc_BackendDAEUtil_getAdjacencyMatrixfromOption(threadData_t *threadData, modelica_metatype _inSyst, modelica_metatype _inIndxType, modelica_metatype _inFunctionTree, modelica_metatype *out_outM, modelica_metatype *out_outMT);
#define boxptr_BackendDAEUtil_getAdjacencyMatrixfromOption omc_BackendDAEUtil_getAdjacencyMatrixfromOption
static const MMC_DEFSTRUCTLIT(boxvar_lit_BackendDAEUtil_getAdjacencyMatrixfromOption,2,0) {(void*) boxptr_BackendDAEUtil_getAdjacencyMatrixfromOption,0}};
#define boxvar_BackendDAEUtil_getAdjacencyMatrixfromOption MMC_REFSTRUCTLIT(boxvar_lit_BackendDAEUtil_getAdjacencyMatrixfromOption)
DLLExport
modelica_metatype omc_BackendDAEUtil_transformBackendDAE(threadData_t *threadData, modelica_metatype _inDAE, modelica_metatype _inMatchingOptions, modelica_metatype _strmatchingAlgorithm, modelica_metatype _strindexReductionMethod);
#define boxptr_BackendDAEUtil_transformBackendDAE omc_BackendDAEUtil_transformBackendDAE
static const MMC_DEFSTRUCTLIT(boxvar_lit_BackendDAEUtil_transformBackendDAE,2,0) {(void*) boxptr_BackendDAEUtil_transformBackendDAE,0}};
#define boxvar_BackendDAEUtil_transformBackendDAE MMC_REFSTRUCTLIT(boxvar_lit_BackendDAEUtil_transformBackendDAE)
DLLExport
modelica_metatype omc_BackendDAEUtil_preOptimizeBackendDAE(threadData_t *threadData, modelica_metatype _inDAE, modelica_metatype _strPreOptModules);
#define boxptr_BackendDAEUtil_preOptimizeBackendDAE omc_BackendDAEUtil_preOptimizeBackendDAE
static const MMC_DEFSTRUCTLIT(boxvar_lit_BackendDAEUtil_preOptimizeBackendDAE,2,0) {(void*) boxptr_BackendDAEUtil_preOptimizeBackendDAE,0}};
#define boxvar_BackendDAEUtil_preOptimizeBackendDAE MMC_REFSTRUCTLIT(boxvar_lit_BackendDAEUtil_preOptimizeBackendDAE)
DLLExport
modelica_metatype omc_BackendDAEUtil_getSolvedSystem(threadData_t *threadData, modelica_metatype _inDAE, modelica_string _fileNamePrefix, modelica_metatype _strPreOptModules, modelica_metatype _strmatchingAlgorithm, modelica_metatype _strdaeHandler, modelica_metatype _strPostOptModules, modelica_metatype *out_outInitDAE, modelica_metatype *out_outInitDAE_lambda0, modelica_metatype *out_inlineData, modelica_metatype *out_outRemovedInitialEquationLst);
#define boxptr_BackendDAEUtil_getSolvedSystem omc_BackendDAEUtil_getSolvedSystem
static const MMC_DEFSTRUCTLIT(boxvar_lit_BackendDAEUtil_getSolvedSystem,2,0) {(void*) boxptr_BackendDAEUtil_getSolvedSystem,0}};
#define boxvar_BackendDAEUtil_getSolvedSystem MMC_REFSTRUCTLIT(boxvar_lit_BackendDAEUtil_getSolvedSystem)
#ifdef __cplusplus
}
#endif
#endif