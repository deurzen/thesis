

/*
WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

This file was generated from state.idl
using RTI Code Generator (rtiddsgen) version 3.1.0.
The rtiddsgen tool is part of the RTI Connext DDS distribution.
For more information, type 'rtiddsgen -help' at a command shell
or consult the Code Generator User's Manual.
*/

#ifndef statePlugin_2087021750_h
#define statePlugin_2087021750_h

#include "state.hh"

struct RTICdrStream;

#ifndef pres_typePlugin_h
#include "pres/pres_typePlugin.h"
#endif

#if (defined(RTI_WIN32) || defined (RTI_WINCE) || defined(RTI_INTIME)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport __declspec(dllexport)
#endif

#define GlobalStatePlugin_get_sample PRESTypePluginDefaultEndpointData_getSample

#define GlobalStatePlugin_get_buffer PRESTypePluginDefaultEndpointData_getBuffer 
#define GlobalStatePlugin_return_buffer PRESTypePluginDefaultEndpointData_returnBuffer

#define GlobalStatePlugin_create_sample PRESTypePluginDefaultEndpointData_createSample 
#define GlobalStatePlugin_destroy_sample PRESTypePluginDefaultEndpointData_deleteSample 

/* --------------------------------------------------------------------------------------
Support functions:
* -------------------------------------------------------------------------------------- */

NDDSUSERDllExport extern GlobalState*
GlobalStatePluginSupport_create_data_w_params(
    const struct DDS_TypeAllocationParams_t * alloc_params);

NDDSUSERDllExport extern GlobalState*
GlobalStatePluginSupport_create_data_ex(RTIBool allocate_pointers);

NDDSUSERDllExport extern GlobalState*
GlobalStatePluginSupport_create_data(void);

NDDSUSERDllExport extern RTIBool 
GlobalStatePluginSupport_copy_data(
    GlobalState *out,
    const GlobalState *in);

NDDSUSERDllExport extern void 
GlobalStatePluginSupport_destroy_data_w_params(
    GlobalState *sample,
    const struct DDS_TypeDeallocationParams_t * dealloc_params);

NDDSUSERDllExport extern void 
GlobalStatePluginSupport_destroy_data_ex(
    GlobalState *sample,RTIBool deallocate_pointers);

NDDSUSERDllExport extern void 
GlobalStatePluginSupport_destroy_data(
    GlobalState *sample);

NDDSUSERDllExport extern void 
GlobalStatePluginSupport_print_data(
    const GlobalState *sample,
    const char *desc,
    unsigned int indent);

/* ----------------------------------------------------------------------------
Callback functions:
* ---------------------------------------------------------------------------- */

NDDSUSERDllExport extern PRESTypePluginParticipantData 
GlobalStatePlugin_on_participant_attached(
    void *registration_data, 
    const struct PRESTypePluginParticipantInfo *participant_info,
    RTIBool top_level_registration, 
    void *container_plugin_context,
    RTICdrTypeCode *typeCode);

NDDSUSERDllExport extern void 
GlobalStatePlugin_on_participant_detached(
    PRESTypePluginParticipantData participant_data);

NDDSUSERDllExport extern PRESTypePluginEndpointData 
GlobalStatePlugin_on_endpoint_attached(
    PRESTypePluginParticipantData participant_data,
    const struct PRESTypePluginEndpointInfo *endpoint_info,
    RTIBool top_level_registration, 
    void *container_plugin_context);

NDDSUSERDllExport extern void 
GlobalStatePlugin_on_endpoint_detached(
    PRESTypePluginEndpointData endpoint_data);

NDDSUSERDllExport extern void    
GlobalStatePlugin_return_sample(
    PRESTypePluginEndpointData endpoint_data,
    GlobalState *sample,
    void *handle);    

NDDSUSERDllExport extern RTIBool 
GlobalStatePlugin_copy_sample(
    PRESTypePluginEndpointData endpoint_data,
    GlobalState *out,
    const GlobalState *in);

/* ----------------------------------------------------------------------------
(De)Serialize functions:
* ------------------------------------------------------------------------- */

NDDSUSERDllExport extern RTIBool
GlobalStatePlugin_serialize_to_cdr_buffer(
    char * buffer,
    unsigned int * length,
    const GlobalState *sample,
    ::dds::core::policy::DataRepresentationId representation
    = ::dds::core::policy::DataRepresentation::xcdr()); 

NDDSUSERDllExport extern RTIBool 
GlobalStatePlugin_deserialize(
    PRESTypePluginEndpointData endpoint_data,
    GlobalState **sample, 
    RTIBool * drop_sample,
    struct RTICdrStream *stream,
    RTIBool deserialize_encapsulation,
    RTIBool deserialize_sample, 
    void *endpoint_plugin_qos);

NDDSUSERDllExport extern RTIBool
GlobalStatePlugin_deserialize_from_cdr_buffer(
    GlobalState *sample,
    const char * buffer,
    unsigned int length);    

NDDSUSERDllExport extern unsigned int 
GlobalStatePlugin_get_serialized_sample_max_size(
    PRESTypePluginEndpointData endpoint_data,
    RTIBool include_encapsulation,
    RTIEncapsulationId encapsulation_id,
    unsigned int current_alignment);

/* --------------------------------------------------------------------------------------
Key Management functions:
* -------------------------------------------------------------------------------------- */
NDDSUSERDllExport extern PRESTypePluginKeyKind 
GlobalStatePlugin_get_key_kind(void);

NDDSUSERDllExport extern unsigned int 
GlobalStatePlugin_get_serialized_key_max_size(
    PRESTypePluginEndpointData endpoint_data,
    RTIBool include_encapsulation,
    RTIEncapsulationId encapsulation_id,
    unsigned int current_alignment);

NDDSUSERDllExport extern unsigned int 
GlobalStatePlugin_get_serialized_key_max_size_for_keyhash(
    PRESTypePluginEndpointData endpoint_data,
    RTIEncapsulationId encapsulation_id,
    unsigned int current_alignment);

NDDSUSERDllExport extern RTIBool 
GlobalStatePlugin_deserialize_key(
    PRESTypePluginEndpointData endpoint_data,
    GlobalState ** sample,
    RTIBool * drop_sample,
    struct RTICdrStream *stream,
    RTIBool deserialize_encapsulation,
    RTIBool deserialize_key,
    void *endpoint_plugin_qos);

/* Plugin Functions */
NDDSUSERDllExport extern struct PRESTypePlugin*
GlobalStatePlugin_new(void);

NDDSUSERDllExport extern void
GlobalStatePlugin_delete(struct PRESTypePlugin *);

#if (defined(RTI_WIN32) || defined (RTI_WINCE) || defined(RTI_INTIME)) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols.
*/
#undef NDDSUSERDllExport
#define NDDSUSERDllExport
#endif

#endif /* statePlugin_2087021750_h */

