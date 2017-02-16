
#include "opj_includes.h"

const char*  opj_version(void) {
	return "1.4.0";
}

opj_dinfo_t*  opj_create_decompress(OPJ_CODEC_FORMAT format) {
	opj_dinfo_t *dinfo = (opj_dinfo_t*)opj_calloc(1, sizeof(opj_dinfo_t));
	if(!dinfo) return NULL;
	dinfo->is_decompressor = true;
	switch(format) {
	case CODEC_J2K:
	case CODEC_JPT:
		/* get a J2K decoder handle */
		dinfo->j2k_handle = (void*)j2k_create_decompress((opj_common_ptr)dinfo);
		if(!dinfo->j2k_handle) {
			opj_free(dinfo);
			return NULL;
		}
		break;
	case CODEC_JP2:
		/* get a JP2 decoder handle */
		dinfo->jp2_handle = (void*)jp2_create_decompress((opj_common_ptr)dinfo);
		if(!dinfo->jp2_handle) {
			opj_free(dinfo);
			return NULL;
		}
		break;
	case CODEC_UNKNOWN:
	default:
		opj_free(dinfo);
		return NULL;
	}

	dinfo->codec_format = format;

	return dinfo;
}

void  opj_destroy_decompress(opj_dinfo_t *dinfo) {
	if(dinfo) {
		/* destroy the codec */
		switch(dinfo->codec_format) {
		case CODEC_J2K:
		case CODEC_JPT:
			j2k_destroy_decompress((opj_j2k_t*)dinfo->j2k_handle);
			break;
		case CODEC_JP2:
			jp2_destroy_decompress((opj_jp2_t*)dinfo->jp2_handle);
			break;
		case CODEC_UNKNOWN:
		default:
			break;
		}
		/* destroy the decompressor */
		opj_free(dinfo);
	}
}

void  opj_set_default_decoder_parameters(opj_dparameters_t *parameters) {
	if(parameters) {
		memset(parameters, 0, sizeof(opj_dparameters_t));
		/* default decoding parameters */
		parameters->cp_layer = 0;
		parameters->cp_reduce = 0;
		parameters->cp_limit_decoding = NO_LIMITATION;

		parameters->decod_format = -1;
		parameters->cod_format = -1;
	}
}

void  opj_setup_decoder(opj_dinfo_t *dinfo, opj_dparameters_t *parameters) {
	if(dinfo && parameters) {
		switch(dinfo->codec_format) {
		case CODEC_J2K:
		case CODEC_JPT:
			j2k_setup_decoder((opj_j2k_t*)dinfo->j2k_handle, parameters);
			break;
		case CODEC_JP2:
			jp2_setup_decoder((opj_jp2_t*)dinfo->jp2_handle, parameters);
			break;
		case CODEC_UNKNOWN:
		default:
			break;
		}
	}
}

opj_image_t*  opj_decode(opj_dinfo_t *dinfo, opj_cio_t *cio) {
	return opj_decode_with_info(dinfo, cio, NULL);
}

opj_image_t*  opj_decode_with_info(opj_dinfo_t *dinfo, opj_cio_t *cio, opj_codestream_info_t *cstr_info) {
	if(dinfo && cio) {
		switch(dinfo->codec_format) {
		case CODEC_J2K:
			return j2k_decode((opj_j2k_t*)dinfo->j2k_handle, cio, cstr_info);
		case CODEC_JPT:
			return j2k_decode_jpt_stream((opj_j2k_t*)dinfo->j2k_handle, cio, cstr_info);
		case CODEC_JP2:
		    return jp2_decode((opj_jp2_t*)dinfo->jp2_handle, cio, cstr_info);
		case CODEC_UNKNOWN:
		default:
			break;
		}
	}
	return NULL;
}

opj_cinfo_t*  opj_create_compress(OPJ_CODEC_FORMAT format) {
	opj_cinfo_t *cinfo = (opj_cinfo_t*)opj_calloc(1, sizeof(opj_cinfo_t));
	if(!cinfo) return NULL;
	cinfo->is_decompressor = false;
	switch(format) {
	case CODEC_J2K:
		/* get a J2K coder handle */
		cinfo->j2k_handle = (void*)j2k_create_compress((opj_common_ptr)cinfo);
		if(!cinfo->j2k_handle) {
			opj_free(cinfo);
			return NULL;
		}
		break;
	case CODEC_JP2:
		/* get a JP2 coder handle */
		cinfo->jp2_handle = (void*)jp2_create_compress((opj_common_ptr)cinfo);
		if(!cinfo->jp2_handle) {
			opj_free(cinfo);
			return NULL;
		}
		break;
	case CODEC_JPT:
	case CODEC_UNKNOWN:
	default:
		opj_free(cinfo);
		return NULL;
	}

	cinfo->codec_format = format;

	return cinfo;
}

void  opj_destroy_compress(opj_cinfo_t *cinfo) {
	if(cinfo) {
		/* destroy the codec */
		switch(cinfo->codec_format) {
		case CODEC_J2K:
			j2k_destroy_compress((opj_j2k_t*)cinfo->j2k_handle);
			break;
		case CODEC_JP2:
			jp2_destroy_compress((opj_jp2_t*)cinfo->jp2_handle);
			break;
		case CODEC_JPT:
		case CODEC_UNKNOWN:
		default:
			break;
		}
		/* destroy the decompressor */
		opj_free(cinfo);
	}
}

void  opj_set_default_encoder_parameters(opj_cparameters_t *parameters) {
	if(parameters) {
		memset(parameters, 0, sizeof(opj_cparameters_t));
		/* default coding parameters */
		parameters->cp_cinema = OFF; 
		parameters->max_comp_size = 0;
		parameters->numresolution = 6;
		parameters->cp_rsiz = STD_RSIZ;
		parameters->cblockw_init = 64;
		parameters->cblockh_init = 64;
		parameters->prog_order = LRCP;
		parameters->roi_compno = -1;		/* no ROI */
		parameters->subsampling_dx = 1;
		parameters->subsampling_dy = 1;
		parameters->tp_on = 0;
		parameters->decod_format = -1;
		parameters->cod_format = -1;
		parameters->tcp_rates[0] = 0;   
		parameters->tcp_numlayers = 0;
		parameters->cp_disto_alloc = 0;
		parameters->cp_fixed_alloc = 0;
		parameters->cp_fixed_quality = 0;

		/* UniPG>> */
#ifdef USE_JPWL
		parameters->jpwl_epc_on = false;
		parameters->jpwl_hprot_MH = -1; /* -1 means unassigned */
		{
			int i;
			for (i = 0; i < JPWL_MAX_NO_TILESPECS; i++) {
				parameters->jpwl_hprot_TPH_tileno[i] = -1; /* unassigned */
				parameters->jpwl_hprot_TPH[i] = 0; /* absent */
			}
		};
		{
			int i;
			for (i = 0; i < JPWL_MAX_NO_PACKSPECS; i++) {
				parameters->jpwl_pprot_tileno[i] = -1; /* unassigned */
				parameters->jpwl_pprot_packno[i] = -1; /* unassigned */
				parameters->jpwl_pprot[i] = 0; /* absent */
			}
		};
		parameters->jpwl_sens_size = 0; /* 0 means no ESD */
		parameters->jpwl_sens_addr = 0; /* 0 means auto */
		parameters->jpwl_sens_range = 0; /* 0 means packet */
		parameters->jpwl_sens_MH = -1; /* -1 means unassigned */
		{
			int i;
			for (i = 0; i < JPWL_MAX_NO_TILESPECS; i++) {
				parameters->jpwl_sens_TPH_tileno[i] = -1; /* unassigned */
				parameters->jpwl_sens_TPH[i] = -1; /* absent */
			}
		};
#endif /* USE_JPWL */
		/* <<UniPG */
	}
}

void  opj_setup_encoder(opj_cinfo_t *cinfo, opj_cparameters_t *parameters, opj_image_t *image) {
	if(cinfo && parameters && image) {
		switch(cinfo->codec_format) {
		case CODEC_J2K:
			j2k_setup_encoder((opj_j2k_t*)cinfo->j2k_handle, parameters, image);
			break;
		case CODEC_JP2:
			jp2_setup_encoder((opj_jp2_t*)cinfo->jp2_handle, parameters, image);
			break;
		case CODEC_JPT:
		case CODEC_UNKNOWN:
		default:
			break;
		}
	}
}

int  opj_encode(opj_cinfo_t *cinfo, opj_cio_t *cio, opj_image_t *image, char *index) {
	if (index != NULL)
		opj_event_msg((opj_common_ptr)cinfo, EVT_WARNING, "Set index to NULL when calling the opj_encode function.\n"
		"To extract the index, use the opj_encode_with_info() function.\n"
		"No index will be generated during this encoding\n");
	return opj_encode_with_info(cinfo, cio, image, NULL);
}

int  opj_encode_with_info(opj_cinfo_t *cinfo, opj_cio_t *cio, opj_image_t *image, opj_codestream_info_t *cstr_info) {
	if(cinfo && cio && image) {
		switch(cinfo->codec_format) {
		case CODEC_J2K:
			return j2k_encode((opj_j2k_t*)cinfo->j2k_handle, cio, image, cstr_info);
		case CODEC_JP2:
			return jp2_encode((opj_jp2_t*)cinfo->jp2_handle, cio, image, cstr_info);
		case CODEC_JPT:
		case CODEC_UNKNOWN:
		default:
			break;
		}
	}
	return false;
}

void  opj_destroy_cstr_info(opj_codestream_info_t *cstr_info) {
	if (cstr_info) {
		int tileno;
		for (tileno = 0; tileno < cstr_info->tw * cstr_info->th; tileno++) {
			opj_tile_info_t *tile_info = &cstr_info->tile[tileno];
			opj_free(tile_info->thresh);
			opj_free(tile_info->packet);
			opj_free(tile_info->tp);
		}
		opj_free(cstr_info->tile);
		opj_free(cstr_info->marker);
		opj_free(cstr_info->numdecompos);
	}
}
