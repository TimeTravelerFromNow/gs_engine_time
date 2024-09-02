/*==============================================================================================================
    * Copyright: 2020 John Jackson 
    * Gunslinger Engine
    * File: gs_asset.h
    * Github: https://github.com/MrFrenik/gs_engine

    All Rights Reserved

    BSD 3-Clause License

    Copyright (c) 2020 John Jackson

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its contributors may be used to 
    endorse or promote products derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIEDi
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=================================================================================================================*/ 

#ifndef GS_ASSET_H
#define GS_ASSET_H

#define GS_ASSET_STR_MAX 1024

// Forward decls.
struct gs_asset_importer_t;
struct gs_asset_record_t;
struct gs_asset_storage_t;

/*
 * TODO(john):
 * 		Create default assets as fallbacks (in case of loading issues)
 * 		Try to load an animation file and animate for 3d shit
 * 		Use dirent.h for file iteration to get around windows garbage
*/

introspect()
typedef enum gs_asset_state
{
	GS_ASSET_STATE_UNLOADED = 0x00,
	GS_ASSET_STATE_LOADED
} gs_asset_state;

// Functions
GS_API_DECL void gs_asset_qualified_name(const char* src, char* dst, size_t sz);

introspect()
typedef struct gs_asset_t
{
    base(gs_object_t)

	// Fields
    uint32_t record_hndl;    // Handle to internal record for asset
	gs_asset_state state;

} gs_asset_t; 

typedef struct gs_asset_handle_t
{ 
    uint32_t hndl;              // Slot array handle to raw asset in importer
    uint32_t importer;     // Slot array handle to raw Importer data in asset manager 
} gs_asset_handle_t;

GS_API_DECL gs_asset_handle_t gs_asset_handle_invalid();
GS_API_DECL gs_asset_handle_t gs_asset_handle_create(const gs_asset_t* asset);
GS_API_DECL gs_asset_t* gs_asset_handle_get(gs_asset_handle_t	* hndl);

introspect()
typedef struct gs_mesh_t 
{
	base(gs_asset_t) 
	
	// Fields
	gs_gfxt_mesh_t mesh;

} gs_mesh_t;

#define gs_mesh_draw(CB, MESH)\
    gs_gfxt_mesh_draw((CB), &((MESH)->mesh))

GS_API_DECL bool gs_mesh_load_resource_from_file(const char* dir, const char* fname, gs_asset_t* out, void* import_options /*gs_gfxt_mesh_import_options_t*/);

introspect()
typedef struct gs_texture_t
{
	base(gs_asset_t)

    serialize({
        return gs_texture_serialize(buffer, this);
    })

    deserialize({
        return gs_texture_deserialize(buffer, this);
    })

	// Fields
    field()
	gs_asset_texture_t texture;

} gs_texture_t; 

GS_API_DECL gs_result gs_texture_serialize(gs_byte_buffer_t* buffer, const gs_object_t* in);
GS_API_DECL gs_result gs_texture_deserialize(gs_byte_buffer_t* buffer, gs_object_t* out); 
GS_API_DECL bool gs_texture_load_resource_from_file(const char* path, gs_asset_t* out, void* user_data);

introspect()
typedef struct gs_font_t
{
	base(gs_asset_t)

	// Fields
	gs_asset_font_t font;

} gs_font_t;

GS_API_DECL bool gs_font_load_resource_from_file(const char* path, gs_asset_t* out, void* user_data);

introspect()
typedef struct gs_sound_t
{
    base(gs_asset_t)

	// Fields
	gs_asset_audio_t audio;

} gs_sound_t;

GS_API_DECL bool gs_sound_load_resource_from_file(const char* path, gs_asset_t* out, void* user_data);

introspect()
typedef struct gs_pipeline_t	
{
    base(gs_asset_t)

    ctor({
        params(void),
        func({
            gs_println("PIPELINE CTOR");
        })
    })

    // Fields
    gs_gfxt_pipeline_t pipeline;
    gs_dyn_array(gs_gfxt_mesh_layout_t) mesh_layout;

} gs_pipeline_t;

GS_API_DECL gs_gfxt_pipeline_t* gs_get_pipeline_raw(GS_GFXT_HNDL hndl, void* user_data)
{
    gs_pipeline_t* pip = (gs_pipeline_t*)hndl;
    return &pip->pipeline;
}

GS_API_DECL bool gs_pipeline_load_resource_from_file(const char* path, gs_asset_t* out, void* user_data);
GS_API_DECL gs_gfxt_mesh_import_options_t gs_pipeline_get_mesh_import_options(const gs_pipeline_t* pipe);

introspect()
typedef struct gs_material_t
{
	base(gs_asset_t)

    ctor({
        params(gs_asset_handle_t pip),
        func({
            gs_pipeline_t* pipe = gs_asset_handle_get(&pip);
            gs_assert(pipe);
            this->pipeline = pip;
            this->material = gs_gfxt_material_create(&(gs_gfxt_material_desc_t){
                .pip_func = {
                    .func = gs_get_pipeline_raw	,
                    .hndl = pipe
                }
            });
            gs_assert(this->material.desc.pip_func.hndl);
        })
    })

	// Fields 
    field()
    gs_asset_handle_t pipeline;

	gs_gfxt_material_t material;

} gs_material_t;

GS_API_DECL void gs_material_set_uniform(gs_material_t* mat, const char* name, void* data); 

#define gs_material_bind(CB, MAT)\
    gs_gfxt_material_bind((CB), &((MAT)->material))

#define gs_material_bind_uniforms(CB, MAT)\
    gs_gfxt_material_bind_uniforms((CB), &((MAT)->material)) 

// VTable functions
GS_API_DECL gs_result gs_material_serialize(gs_byte_buffer_t* buffer, gs_object_t* in);
GS_API_DECL gs_result gs_material_deserialize(gs_byte_buffer_t* buffer, gs_object_t* out); 

// Should these be assets as well?
/*
typedef struct material_instance_t
{
    base(gs_asset_t);

    uint32_t parent;            // Asset handle to parent material/instance
    gs_byte_buffer_t overrides; // Override of uniform data (don't want to copy data...)
} material_instance_t;
*/ 

typedef struct gs_asset_record_t
{
	base(gs_asset_t)

	// Fields
	uint32_t hndl;		        // Handle to asset slot array in storage
    char path[GS_ASSET_STR_MAX];   // Absolute path to asset on disk
    gs_uuid_t uuid;             // UUID for asset
    char name[GS_ASSET_STR_MAX];   // Qualified name for asset
} gs_asset_record_t	;

#define GS_ASSET_IMPORTER_FILE_EXTENSIONS_MAX 10
#define GS_ASSETS_FILE_EXTENSION_MAX_LENGTH 32

typedef struct gs_asset_importer_desc_t
{
	bool (* load_resource_from_file)(const char* path, gs_asset_t* out, void* user_data);
	char* file_extensions[GS_ASSET_IMPORTER_FILE_EXTENSIONS_MAX	];
	size_t file_extensions_size;
    char file_extension[GS_ASSETS_FILE_EXTENSION_MAX_LENGTH	];
} gs_asset_importer_desc_t;

typedef struct gs_asset_importer_t
{
	base(gs_object_t)

	gs_slot_array(gs_asset_record_t	) records;		// Slot array of asset records
	gs_hash_table(uint64_t, uint32_t) uuid2id;  // Lookup mapping from uuid to record slot id
	gs_hash_table(uint64_t, uint32_t) name2id;  // Lookup mapping from name to record slot id
	gs_slot_array(gs_asset_t*) assets;			    // Slot array of raw asset data (could try to come up with a way to not malloc this)

	bool (* load_resource_from_file)(const char* path, gs_asset_t* out, void* user_data);
	uint64_t asset_cls_id;
	size_t cls_sz;
    char file_extension[GS_ASSETS_FILE_EXTENSION_MAX_LENGTH	];
} gs_asset_importer_t;

typedef struct gs_asset_manager_t
{
	base(gs_object_t)

	// Fields
	char root_path[GS_ASSET_STR_MAX];
	gs_slot_array(gs_asset_importer_t*) importers;	    // Slot array of asset data
	gs_hash_table(uint64_t, uint32_t) cid2importer; // Mapping from cls id to importer data
	gs_hash_table(uint64_t, uint32_t) fe2importer;  // Mapping from file extension to importer data

} gs_asset_manager_t;

GS_API_DECL void gs_assets_init(gs_asset_manager_t* am, const char* root_path);
GS_API_DECL const char* assets_get_internal_file_extension(gs_asset_manager_t* am, const char* ext);
GS_API_DECL gs_asset_handle_t gs_assets_import(gs_asset_manager_t* am, const char* path, void* user_data, bool serialize_to_disk);
GS_API_DECL gs_asset_handle_t gs_assets_add_to_database(gs_asset_manager_t* am, gs_asset_t* asset, const char* dir, const char* name, bool serialize_to_disk);
GS_API_DECL gs_result gs_assets_serialize_asset(gs_asset_manager_t* am, const char* path, const gs_asset_t* in);
GS_API_DECL gs_result gs_assets_deserialize_asset(gs_asset_manager_t* am, const char*path, gs_asset_t* out);
GS_API_DECL void* gs_assets_get_data_internal(uint64_t cls_id);
GS_API_DECL  const gs_asset_t* _gs_assets_get_w_name_internal(const gs_asset_manager_t* am, uint64_t cid, const char* name);
GS_API_DECL void _gs_assets_register_importer_internal(gs_asset_manager_t* am, uint64_t cid, size_t cls_sz, gs_asset_importer_desc_t* desc); 

#define gs_assets_register_importer(ASSETS, T, DESC)\
	_gs_assets_register_importer_internal(ASSETS, gs_obj_sid(T), sizeof(T), DESC);

#define gs_assets_getp(ASSETS, T, NAME)\
	_gs_assets_get_w_name_internal(ASSETS, gs_hash_str64(gs_to_str(T)), NAME)

// Mechanism for getting asset manager instance
#define gs_asset_manager_instance() _g_asset_manager

#ifdef GS_ASSET_IMPL

// Global instance of asset manager
gs_asset_manager_t* _g_asset_manager = NULL; 

// Maps resource file extension to internal engine file extension
GS_API_DECL const char* assets_get_internal_file_extension(gs_asset_manager_t* am, const char* ext)
{
    // Find importer from file extension mapping
    uint64_t hash = gs_hash_str64(ext);
    bool exists = gs_hash_table_exists(am->fe2importer, hash);
    if (!exists)
    {
        gs_println("warning::gs_asset_manager_t::assets_get_internal_file_extension::%s not registered", ext);
        return "ass"; 
    } 

    // Get importer
    gs_asset_importer_t* importer = gs_slot_array_get(am->importers, gs_hash_table_get(am->fe2importer, hash)); 
    return importer->file_extension; 
}

GS_API_DECL const gs_asset_t* _gs_assets_get_w_name_internal(const gs_asset_manager_t* am, uint64_t cid, const char* name)
{
	if (!gs_hash_table_exists(am->cid2importer, cid)) 
	{
		gs_timed_action(60, {
			gs_println("error::gs_asset_manager_t	::getp:: asset type t doesn't exist: %s", name);
		});
		return NULL;
	}

	// Get the storage
	uint32_t shndl = gs_hash_table_get(am->cid2importer, cid);
	gs_asset_importer_t* importer = gs_slot_array_get(am->importers, shndl);
	gs_assert(importer);

	// Get the asset from storage by name
	// TODO(john): Need to load asset if not loaded by default yet	
	uint64_t hash = gs_hash_str64(name);	
	if (!gs_hash_table_exists(importer->name2id, hash))
	{
		gs_timed_action(60, {
			gs_println("error::gs_asset_manager_t	::getp::asset doesn't exist: %s", name);
		});
		return NULL;
	}

	uint32_t rhndl = gs_hash_table_get(importer->name2id, hash);
	const gs_asset_record_t* record = gs_slot_array_getp(importer->records, rhndl);
	gs_assert(record);

	const gs_asset_t* asset = gs_slot_array_get(importer->assets, record->hndl);
	return asset;
}

GS_API_DECL void _gs_assets_register_importer_internal(gs_asset_manager_t* am, uint64_t cid, size_t cls_sz, gs_asset_importer_desc_t* desc)
{
	gs_asset_importer_t* importer = gs_malloc_init(gs_asset_importer_t	);	
	importer->load_resource_from_file = desc->load_resource_from_file;
	importer->asset_cls_id = cid;
	importer->cls_sz = cls_sz;
    memcpy(importer->file_extension, desc->file_extension ? desc->file_extension : "ass", GS_ASSETS_FILE_EXTENSION_MAX_LENGTH);
	uint32_t hndl = gs_slot_array_insert(am->importers, importer);
	gs_hash_table_insert(am->cid2importer, cid, hndl);
	uint32_t ct = desc->file_extensions_size ? desc->file_extensions_size / sizeof(char*) : 0;
	for (uint32_t i = 0; i < ct; ++i)
	{
		gs_hash_table_insert(am->fe2importer, gs_hash_str64(desc->file_extensions[i]), hndl);
	}
}

GS_API_DECL gs_asset_handle_t gs_assets_import(gs_asset_manager_t* am, const char* path, void* user_data, bool serialize_to_disk)	
{
	// Create record for asset, set path to asset using qualified name
	gs_asset_record_t record = {0};

    // Asset handle to create and return
    gs_asset_handle_t asset_hndl = gs_asset_handle_invalid();

	// Get importer from importers
	gs_transient_buffer(FILE_EXT, 10);
	gs_platform_file_extension(FILE_EXT, 10, path);

	if (!gs_hash_table_exists(am->fe2importer, gs_hash_str64(FILE_EXT)))
	{
        gs_println("error::asset_import::impoter does not exist for file extension %s", FILE_EXT);
		return asset_hndl;
	}

	// Get asset storage
	uint32_t importer_hndl = gs_hash_table_get(am->fe2importer, gs_hash_str64(FILE_EXT));
	gs_asset_importer_t	* importer = gs_slot_array_get(am->importers, importer_hndl); 

	// Get class id from storage
	uint64_t id = importer->asset_cls_id;

	// Get absolute path to asset
	gs_snprintfc(PATH, GS_ASSET_STR_MAX, "%s/%s", am->root_path, path);

	// Get qualified name of asset
	gs_transient_buffer(QUAL_NAME, GS_ASSET_STR_MAX);
	gs_asset_qualified_name(path, QUAL_NAME, GS_ASSET_STR_MAX);
	memcpy(record.name, QUAL_NAME, GS_ASSET_STR_MAX);

	// Create final save path for asset
	gs_transient_buffer(FINAL_PATH_TMP, GS_ASSET_STR_MAX);
	gs_transient_buffer(FINAL_PATH, GS_ASSET_STR_MAX);
	gs_snprintf(FINAL_PATH_TMP, GS_ASSET_STR_MAX, "%s/%s", am->root_path, QUAL_NAME);
	gs_util_string_replace_delim(FINAL_PATH_TMP, (FINAL_PATH + 1), GS_ASSET_STR_MAX, '.', '/');
	FINAL_PATH[0] = '.';

	// Get file extension from registered mappings
	const char* file_ext = assets_get_internal_file_extension(am, FILE_EXT);
	gs_snprintf(record.path, GS_ASSET_STR_MAX, "%s.%s", FINAL_PATH, file_ext);

       // Generate uuid for asset
	record.uuid = gs_platform_uuid_generate(); 

	// Need to construct asset type here using vtable
    gs_asset_t* asset = gs_obj_newid(id); 
	gs_assert(asset); 

	// Construct raw asset (this will also place into storage and give asset the record's handle)
	bool loaded = importer->load_resource_from_file(PATH, asset, user_data); 
	if (loaded)
	{
        gs_println("asset_import::imported %s to %s", path, record.path);

		// Insert into data array
		uint32_t hndl = gs_slot_array_insert(importer->assets, asset);

		// Set up tables
		gs_transient_buffer(UUID_BUF, 34);
		gs_platform_uuid_to_string(UUID_BUF, &record.uuid);
		gs_hash_table_insert(importer->uuid2id, gs_hash_str64(UUID_BUF), hndl);
		gs_hash_table_insert(importer->name2id, gs_hash_str64(record.name), hndl); 

        // Assign asset handle to record
        record.hndl = hndl;

		// Store record in storage
		uint32_t rhndl = gs_slot_array_insert(importer->records, record);

		// Set asset record hndl
		asset->record_hndl = rhndl; 

		// Serialize asset to disk
        if (serialize_to_disk)
        {
		    gs_assets_serialize_asset(am, record.path, asset);
        }

        // Set asset handle data
        asset_hndl.hndl = hndl; 
        asset_hndl.importer = importer_hndl;
	}

    return asset_hndl;
}

GS_API_DECL gs_asset_handle_t gs_assets_add_to_database(gs_asset_manager_t* am, gs_asset_t* asset, const char* dir, const char* name, bool serialize_to_disk)
{
	// Create record for asset, set path to asset using qualified name
	gs_asset_record_t record = gs_default_val();

    // Asset handle to create and return
    gs_asset_handle_t asset_hndl = gs_asset_handle_invalid();

    if (!asset)
    { 
        gs_println("error::gs_assets_add_to_database::asset id %zu does not exist.", gs_obj_id(asset));
        return asset_hndl;
    } 

    if (!gs_hash_table_key_exists(am->cid2importer, gs_obj_id(asset)))
    {
        gs_println("error::gs_assets_add_to_database::asset id %zu does not exist.", gs_obj_id(asset));
        return asset_hndl;
    }

	// Get asset storage
	uint32_t importer_hndl = gs_hash_table_get(am->cid2importer, gs_obj_id(asset));
	gs_asset_importer_t* importer = gs_slot_array_get(am->importers, importer_hndl); 

	// Get class id from storage
	uint64_t id = importer->asset_cls_id;

    // Get relative path to asset
    gs_snprintfc(REL_PATH, GS_ASSET_STR_MAX, "%s/%s.%s", dir, name, importer->file_extension);

    // Get absolute directory
    gs_snprintfc(ABS_DIR, GS_ASSET_STR_MAX, "%s/%s/", am->root_path, dir);

	// Get absolute path to asset
	gs_snprintfc(ABS_PATH, GS_ASSET_STR_MAX, "%s/%s", am->root_path, REL_PATH); 

    // Copy final path
	gs_snprintf(record.path, GS_ASSET_STR_MAX, "%s", ABS_PATH);

	// Get qualified name of asset
	gs_transient_buffer(QUAL_NAME, GS_ASSET_STR_MAX);
	gs_asset_qualified_name(REL_PATH, QUAL_NAME, GS_ASSET_STR_MAX);
	memcpy(record.name, QUAL_NAME, GS_ASSET_STR_MAX); 

    // Generate uuid for asset
	record.uuid = gs_platform_uuid_generate(); 

    // Add to database
    gs_println("gs_assets_add_to_database::asset %s", name);

    // Insert into data array
    uint32_t hndl = gs_slot_array_insert(importer->assets, asset);

    // Set up tables
    gs_transient_buffer(UUID_BUF, 34);
    gs_platform_uuid_to_string(UUID_BUF, &record.uuid);
    gs_hash_table_insert(importer->uuid2id, gs_hash_str64(UUID_BUF), hndl);
    gs_hash_table_insert(importer->name2id, gs_hash_str64(record.name), hndl); 

    // Assign asset handle to record
    record.hndl = hndl;

    // Store record in storage
    uint32_t rhndl = gs_slot_array_insert(importer->records, record);

    // Set asset record hndl
    asset->record_hndl = rhndl; 

    // Serialize asset to disk
    if (serialize_to_disk)
    {
        DIR* _dir = opendir(ABS_DIR);
        if (!_dir) {
            mkdir(ABS_DIR, S_IWRITE | S_IREAD);
        }

        gs_assets_serialize_asset(am, record.path, asset);
    }

    // Set asset handle data
    asset_hndl.hndl = hndl; 
    asset_hndl.importer = importer_hndl;

    // Return asset handle
    return asset_hndl;
}

GS_API_DECL gs_result gs_assets_serialize_asset(gs_asset_manager_t* am, const char* path, const gs_asset_t* in)
{ 
    // Get class id
    uint64_t id = gs_obj_id(in);

    // Get asset importer based on type of asset
    uint32_t sid = gs_hash_table_get(am->cid2importer, id);
    gs_asset_importer_t	* importer = gs_slot_array_get(am->importers, sid);
    const gs_asset_record_t	* record = gs_slot_array_getp(importer->records, in->record_hndl);

	gs_byte_buffer_t bb = gs_byte_buffer_new();

	// === Object Header === //
	gs_byte_buffer_write(&bb, uint64_t, id);	// Class id (not sure about this) should write out class name instead?

	// === Asset Header === //
	gs_byte_buffer_write(&bb, gs_uuid_t, record->uuid);
	gs_byte_buffer_write_str(&bb, record->name);

	// Serialize asset data 
    gs_obj_serialize_func serialize = gs_obj_func_w_id(id, gs_obj_serialize);
    gs_result res = (*serialize)(&bb, in);
		
	// Default serialization if no serializer provider
	if (res == GS_RESULT_INCOMPLETE)
	{
        res = gs_object_serialize_default(&bb, in);
	} 

	// Write to file
	gs_byte_buffer_write_to_file(&bb, path);

	// Free buffer
	gs_byte_buffer_free(&bb);

	return GS_RESULT_SUCCESS;
}

/*
gs_result gs_asset_manager_t	::deserialize_asset(const char* path, gs_asset_t* out)
{ 
    // NOTE(john): Object should already be instantiated.
    gs_assert(out);

    // Cache global asset manager instance
    gs_asset_manager_t	* am = gs_asset_manager_instance();

	gs_byte_buffer_t bb = gs_byte_buffer_new();

	// Read buffer from file
	gs_byte_buffer_read_from_file(&bb, path);

	// === Object Header === //
	gs_byte_buffer_readc(&bb, uint64_t, id);	// Class id

    // Get storage based on id, need a record handle somehow. Don't have that yet.
    // Let's assume the asset has a record handle. It should, since we place that on load.  
	uint32_t sid = am->cid2assets.get(id);
    asset_storage_base_t* storage = am->assets.get(sid);
    const gs_asset_record_t	* record = storage->records.getp(out->record_hndl);

	// === Asset Header === //
	gs_byte_buffer_read(&bb, gs_uuid_t, &record->uuid);
	gs_byte_buffer_read_str(&bb, (char*)record->name);

	// Serialize asset data 
	gs_result res = out->deserialize(&bb);	
	
	// Default serialization if no serializer provider
	if (res == GS_RESULT_INCOMPLETE)
	{
		gs_object_t	::deserialize_default(&bb, out);
	}

	// Free buffer
	gs_byte_buffer_free(&bb);

	return GS_RESULT_SUCCESS;
} 
*/

void gs_asset_recurse_dir(const char* path, DIR* dir)
{
	struct dirent* ent = NULL;
	while ((ent = readdir(dir)) != NULL)
	{
		switch (ent->d_type)
		{
			case DT_REG:
			{
				// Files
				gs_println("reg: %s", ent->d_name);
			} break;

			case DT_DIR:
			{
				// Skip these
				if (gs_string_compare_equal(ent->d_name, ".") || gs_string_compare_equal(ent->d_name, ".."))
				{
					continue;
				}
				
				gs_snprintfc(DIRPATH, GS_ASSET_STR_MAX, "%s/%s", path, ent->d_name);
				gs_println("dir: %s", DIRPATH);
				DIR* cdir = opendir(DIRPATH);
				if (cdir)
				{
					gs_asset_recurse_dir(DIRPATH, cdir);
				}
				closedir(cdir);
			} break;

			case DT_LNK:
			{
				gs_println("link: %s", ent->d_name);
			} break;

			default:
			{
				gs_println("def: %s", ent->d_name);
			} break;
		}
	}
}

// Set root path, begin importing all assets
GS_API_DECL void gs_assets_init(gs_asset_manager_t* am, const char* path)
{
    // Set global asset manager instance
    _g_asset_manager = am;

	// Clear all previous records, if necessary
	gs_transient_buffer(TMP, GS_ASSET_STR_MAX);
	gs_snprintf(TMP, GS_ASSET_STR_MAX, "%s", path);
	memcpy(am->root_path, TMP, GS_ASSET_STR_MAX);

	// Register texture importer
	gs_assets_register_importer(am, gs_texture_t, (&(gs_asset_importer_desc_t){
		.load_resource_from_file = gs_texture_load_resource_from_file,
		.file_extensions = {"png", "jpg"},
		.file_extensions_size = 2 * sizeof(char*),
        .file_extension = "tex"
	}));

    // Register audio importer
    gs_assets_register_importer(am, gs_sound_t, (&(gs_asset_importer_desc_t){
        .load_resource_from_file = gs_sound_load_resource_from_file,
        .file_extensions = {"ogg", "wav", "mp3"}, 
        .file_extensions_size = 3 * sizeof(char*),
        .file_extension = "aud"
    })); 

    // Register pipeline importer
    gs_assets_register_importer(am, gs_pipeline_t, (&(gs_asset_importer_desc_t){
        .load_resource_from_file = gs_gfxt_pipeline_load_from_file,
        .file_extensions = {"sf"}, 
        .file_extensions_size = 1 * sizeof(char*),
        .file_extension = "pip"
    })); 

    // Register material importer
    gs_assets_register_importer(am, gs_material_t, (&(gs_asset_importer_desc_t){
        .file_extension = "mat"
    })); 

    // Register mesh importer
    gs_assets_register_importer(am, gs_mesh_t, (&(gs_asset_importer_desc_t){
        .load_resource_from_file = gs_mesh_load_resource_from_file,
        .file_extensions = {"glb", "gltf"}, 
        .file_extensions_size = 2 * sizeof(char*),
        .file_extension = "msh"
    })); 

	// Register font importer
	gs_assets_register_importer(am, gs_font_t, (&(gs_asset_importer_desc_t) {
		.load_resource_from_file = gs_font_load_resource_from_file, 
		.file_extensions = {"ttf", "otf"},
		.file_extensions_size = 2 * sizeof(char*),
		.file_extension = "fnt"
	}));

	// Open directory
	DIR* dir = opendir(path);
	if (!dir) {
		gs_println("error::gs_asset_manager_t	::init::could not open path: %s", path);
		return;
	}

    // Recursive through directory structure for assets
	gs_asset_recurse_dir(path, dir);

	// Close dir
	closedir(dir);
}

typedef struct string_split_t 
{
	uint32_t start;
	uint32_t count;
} string_split_t;

GS_API_DECL void gs_asset_qualified_name(const char* src, char* dst, size_t sz)
{
    // Need to remove extension
    uint32_t idx = 0;
    gs_transient_buffer(TMP, 1024);
    for (uint32_t i = gs_string_length(src) - 1; i >= 0; --i) {
        if (src[i] == '.') {
            idx = i;
            break;
        }
    }
    gs_util_string_substring(src, TMP, 1024, 0, idx);

    // Split string
    gs_dyn_array(string_split_t) splits = NULL;
    gs_dyn_array_reserve(splits, 1);

    // Take in delimiter character, then split based on that
    size_t len = gs_string_length(TMP);
    uint32_t start = 0;
	uint32_t rem = 0;
    for (uint32_t i = 0; i < len; ++i)
    {
        if (TMP[i] == '/') {
            string_split_t split = gs_default_val();
			split.start = start;
			split.count = i - start;
            gs_dyn_array_push(splits, split);
            start = i;
			rem = len - start;
        }
    }
	// Push last bit into split
	string_split_t split = {0};
	split.start = start;
	split.count = rem;
	gs_dyn_array_push(splits, split);

    // If no splits, then push back entire string into split
    if (gs_dyn_array_empty(splits)) {
        string_split_t split = gs_default_val();
		split.start = 0; split.count = len;
        gs_dyn_array_push(splits, split);
    }
	else
	{
		// For each split, print
		uint32_t c = 0;
		gs_for_range_i(gs_dyn_array_size(splits))
		{
			string_split_t* s = &splits[i];
			gs_transient_buffer(TMP2, GS_ASSET_STR_MAX);
			gs_transient_buffer(TMP3, GS_ASSET_STR_MAX);
			memcpy(TMP2, (TMP + s->start), s->count);
			gs_util_string_replace_delim(TMP2, TMP3, GS_ASSET_STR_MAX, '/', '.');
			memcpy((dst + c), TMP3, s->count);
			c += s->count;
		}
		// Need the last remainder of the string as well
		gs_dyn_array_free(splits);
	}
}

//=======[ Asset Handle ]==============================================================

GS_API_DECL gs_asset_handle_t gs_asset_handle_invalid()
{
    return (gs_asset_handle_t){
        .hndl = UINT32_MAX,
        .importer = UINT32_MAX
    };
}

GS_API_DECL gs_asset_handle_t gs_asset_handle_create(const gs_asset_t* asset)
{ 
    // Get asset manager instance
    gs_asset_manager_t* am = gs_asset_manager_instance(); 

    // Get asset importer based on type of asset
    const uint32_t id = gs_obj_id(asset);
    uint32_t iid = gs_hash_table_get(am->cid2importer, id);
    gs_asset_importer_t	* importer = gs_slot_array_get(am->importers, iid);

    // Get record for asset
    const gs_asset_record_t* record = gs_slot_array_getp(importer->records, asset->record_hndl); 

    // Construct asset handle
    gs_asset_handle_t hndl = (gs_asset_handle_t	){
        .hndl = record->hndl, 
        .importer = iid
    };

    return hndl;
}

GS_API_DECL gs_asset_t* gs_asset_handle_get(gs_asset_handle_t* hndl)
{
    // Get raw asset from slot handles 
    const gs_asset_manager_t* am = gs_asset_manager_instance();
    const gs_asset_importer_t* importer = gs_slot_array_get(am->importers, hndl->importer);
    return gs_slot_array_get(importer->assets, hndl->hndl);
}

//=======[ Texture ]==================================================================

GS_API_DECL bool gs_texture_load_resource_from_file(const char* path, gs_asset_t* out, void* user_data)
{
	// Need to load up texture data, store in storage (slot array), then return pointer to asset for serialization.
	gs_graphics_texture_desc_t* desc = (gs_graphics_texture_desc_t*)user_data;		

	// Load texture data from file, keep data	
	gs_texture_t* t = (gs_texture_t*)out;
	gs_asset_texture_t* tex = &t->texture;
	bool loaded = gs_asset_texture_load_from_file(path, tex, desc, false, true);

	if (!loaded)
	{
		// Error
		gs_println("error::assets_texture_t_load_resource_from_file:: texture: %s not loaded.", path);
		gs_free(t);
		return false;
	}

	return true;
} 

GS_API_DECL gs_result gs_texture_serialize(gs_byte_buffer_t* buffer, const gs_object_t* in)
{
    gs_println("SERIALIZE!");
    return GS_RESULT_INCOMPLETE;
	const gs_texture_t* t = (gs_texture_t*)in;
	const gs_asset_texture_t* tex = &t->texture;

    // Verify that data is available first in desc
    if (!tex->desc.data) 
	{
        gs_println("error:assets_texture_t::serialize:: texture desc data is NULL");
        return GS_RESULT_FAILURE;
    }

    // Write out desc information
    gs_byte_buffer_write(buffer, uint32_t, tex->desc.width);                                 // Width
    gs_byte_buffer_write(buffer, uint32_t, tex->desc.height);                                // Height
    gs_byte_buffer_write(buffer, gs_graphics_texture_format_type, tex->desc.format);         // Texture Format
    gs_byte_buffer_write(buffer, gs_graphics_texture_wrapping_type, tex->desc.wrap_s);       // S Wrap
    gs_byte_buffer_write(buffer, gs_graphics_texture_wrapping_type, tex->desc.wrap_t);       // T Wrap
    gs_byte_buffer_write(buffer, gs_graphics_texture_filtering_type, tex->desc.min_filter);  // Min Filter Format
    gs_byte_buffer_write(buffer, gs_graphics_texture_filtering_type, tex->desc.mag_filter);  // Mag Filter Format
    gs_byte_buffer_write(buffer, uint32_t, tex->desc.num_mips);                              // Num Mips

    // TODO(): Need to switch on format type to get this information
    uint32_t num_comps = 4;
    size_t sz = tex->desc.width * tex->desc.height * 4;

    // Write out data
    gs_byte_buffer_write_bulk(buffer, tex->desc.data, sz);

	// Serialize texture data
	return GS_RESULT_SUCCESS;
}

GS_API_DECL gs_result gs_texture_deserialize(gs_byte_buffer_t* buffer, gs_object_t	* out)
{
    gs_println("DESERIALIZE!");
    return GS_RESULT_INCOMPLETE;
	// Deserialize texture data
	gs_texture_t* t = (gs_texture_t*)out;
	gs_asset_texture_t* tex = &t->texture;

    // Read in desc information
    gs_byte_buffer_read(buffer, uint32_t, &tex->desc.width);                                 // Width
    gs_byte_buffer_read(buffer, uint32_t, &tex->desc.height);                                // Height
    gs_byte_buffer_read(buffer, gs_graphics_texture_format_type, &tex->desc.format);         // Texture Format
    gs_byte_buffer_read(buffer, gs_graphics_texture_wrapping_type, &tex->desc.wrap_s);       // S Wrap
    gs_byte_buffer_read(buffer, gs_graphics_texture_wrapping_type, &tex->desc.wrap_t);       // T Wrap
    gs_byte_buffer_read(buffer, gs_graphics_texture_filtering_type, &tex->desc.min_filter);  // Min Filter Format
    gs_byte_buffer_read(buffer, gs_graphics_texture_filtering_type, &tex->desc.mag_filter);  // Mag Filter Format
    gs_byte_buffer_read(buffer, uint32_t, &tex->desc.num_mips);                              // Num Mips

    // TODO(): Need to switch on format type to get this information
    uint32_t num_comps = 4;
    size_t sz = tex->desc.width * tex->desc.height * 4;

    // Allocate texture data
    *tex->desc.data = gs_malloc(sz * GS_GRAPHICS_TEXTURE_DATA_MAX);

    // Read in texture data
    gs_byte_buffer_read_bulk(buffer, &tex->desc.data, sz);

    // Construct graphics texture handle
    tex->hndl = gs_graphics_texture_create(&tex->desc);

    // Free texture data
    gs_free(tex->desc.data);
	
	return GS_RESULT_SUCCESS;
}

//=======[ Audio ]====================================================================

GS_API_DECL bool gs_sound_load_resource_from_file(const char* path, gs_asset_t* out, void* user_data)
{ 
    gs_sound_t* sound = (gs_sound_t*)out;  
	return gs_asset_audio_load_from_file(path, &sound->audio);
}

//=======[ Font ]=====================================================================

GS_API_DECL bool gs_font_load_resource_from_file(const char* path, gs_asset_t* out, void* user_data)
{
	// Need to load up data, store in storage (slot array), then return pointer to asset for serialization.
	gs_font_t* font = (gs_font_t*)out;
	return gs_asset_font_load_from_file(path, &font->font, user_data ? *((uint32_t*)user_data) : 12);
}

//=======[ Mesh ]=====================================================================

GS_API_DECL bool gs_mesh_load_resource_from_file(const char* dir, const char* fname, gs_asset_t* out, void* import_options /*gs_gfxt_mesh_import_options_t*/)
{
	// Need to load up data, store in storage (slot array), then return pointer to asset for serialization.
	gs_mesh_t* mesh = (gs_mesh_t*)out;
	mesh->mesh = gs_gfxt_mesh_load_from_file(dir, fname, import_options);
	return true;
} 

//=======[ Material ]================================================================= 

GS_API_DECL void gs_material_set_uniform(gs_material_t* mat, const char* name, void* data)
{
    gs_gfxt_material_set_uniform(&mat->material, name, data);
}

#endif // GS_ASSET_IMPL
#endif // GS_ASSET_H








