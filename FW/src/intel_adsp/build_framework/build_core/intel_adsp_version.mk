# SPDX-License-Identifier: BSD-3-Clause
#
# Copyright(c) 2021 Intel Corporation. All rights reserved.


ifndef _INTEL_ADSP_VERSION_MK_
_INTEL_ADSP_VERSION_MK_ :=1


# define the default INTEL ADSP API version
#<--
INTEL_ADSP_API_VERSION ?= 4.5.0


# gets version numbers given an INTEL_ADSP_API_VERSION value
#<--
override _MAJOR_API_VERSION_NUMBER := $(word 1,$(subst .,,$(subst .,. ,$(INTEL_ADSP_API_VERSION))))
override _MIDDLE_API_VERSION_NUMBER = $(word 2,$(subst .,,$(subst .,. ,,$(INTEL_ADSP_API_VERSION))))
override _MINOR_API_VERSION_NUMBER = $(word 3,$(subst .,,$(subst .,. ,,$(INTEL_ADSP_API_VERSION))))
#-->

endif

# changes log
#############
# API version 4.5.0 :
# - Increasing MODULE_PASS_BUFFER_SIZE to 320 to reserve additional space for backward compatibile future changes in ModuleAdapter and ModuleInstance
# - adding support for module with no output sink exposed (HungryRTSink for AcaModule)
# API version 4.4.0 :
# - Restore flags added in ModuleInstance
# API version 4.3.0 :
# - Internal extraction probes from sue.
# API version 4.2.0 :
# - Reference queue buffers allocation is moved from Pipeline memory pool to module .bss
# - ModuleAdapter needs to manage queues now
# - new header updated by FDK tool - build/module_design_config.h contains config for ProcessingModule template data (INPUT_NUMBER, OUTPUT_NUMBER, reference queues buffer size)
# API version 4.1.0 :
# - Moved some counters out of MIPerfData resulting in change of ModuleAdapter size.
# API version 4.0.1 :
# - Add Notification Service in FDK module API.
# API version 4.0.0 :
# - Add support for heterogeneous audio format of module pins:
#   + Re-implement the ModuleInitialSettings class. It now gives access to a collection of key-value pairs and rely on the ADSP system for retrieval of the settings values.
#   + Rename previous ModuleInitialSettings struct into LegacyModuleInitialSettings.
#   + new ModuleInitialSettings class is put in a new file module_initial_settings.h
#   + Add access to audio format info per pin in ModuleInitialSettings.
#   + Modify GetPrerequisites() function interface and ProcessingModulePrerequisites struct.
# API version 3.0.0 :
# - move log_priority.h file into intel_adsp/include/ipc directory
# API version 2.0.0 :
# - Update doc to remove CHANNELS_BLOCKS_INTERLEAVING as it is not supported.
# - __FILE_ID__ definition is now automatically generated by the build system and don't have to be set in module source code.
# API version 1.0.0 :
# - USING_LOGGER is not supported anymore. Remove using_log module example.
# - Provide AdspLogHandle, AdspModuleHandle, AdspLogPriority and AdspSystemService types (in global space) for C implementation.
# - LogHandle, SystemService and ModulePlaceHolder types moved into intel_adsp namespace (previously were within global scope).
#   This change may imply some slight rework of existing modules source code.
# API version 0.6.0 :
# - enable audio format retrieval from module.
# - moving location of module_initial_settings.h to intel_adsp/include/ipc
# API version 0.5.0 :
# add support for preprocessing modules.
# API version 0.4.1 :
# - add API version number in module memory mapping.
# - add support of module building for cannonlake and kabylake.
# - fix some header file inclusions.
# API version 0.4.0 :
# initial revision
