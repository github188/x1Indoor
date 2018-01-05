
/* 
 * h264bitstream - a library for reading and writing H.264 video
 * Copyright (C) 2005-2007 Auroras Entertainment, LLC
 * Copyright (C) 2008-2011 Avail-TVN
 * 
 * Written by Alex Izvorski <aizvorski@gmail.com> and Alex Giladi <alex.giladi@gmail.com>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "bs.h"
#include "logic_H264Stream_analysis.h"

static h264_stream_t* g_H264_Stream = NULL;
static int g_SPSTable = 0;

int get_h264_rect(int *witgh, int* heigth)
{
	if (g_H264_Stream)
	{
		#if _ONLY_ANALYSIS_SPS_
		sps_t* sps = g_H264_Stream->sps;
		#else
		sps_t* sps = g_H264_Stream->sps_table[g_SPSTable];
		#endif
		*witgh = (sps->pic_width_in_mbs_minus1 + 1)*16;
		*heigth = (sps->pic_height_in_map_units_minus1 + 1)*16;
		return 0;
	}
	else
		return -1;
}

//7.3.2.1.1 Scaling list syntax
void read_scaling_list(bs_t* b, int* scalingList, int sizeOfScalingList, int useDefaultScalingMatrixFlag )
{
    int j;
    if(scalingList == NULL)
    {
        return;
    }
 
    int lastScale = 8;
    int nextScale = 8;
    for( j = 0; j < sizeOfScalingList; j++ )
    {
        if( nextScale != 0 )
        {
            int delta_scale = bs_read_se(b);
            nextScale = ( lastScale + delta_scale + 256 ) % 256;
            useDefaultScalingMatrixFlag = ( j == 0 && nextScale == 0 );
        }
        scalingList[ j ] = ( nextScale == 0 ) ? lastScale : nextScale;
        lastScale = scalingList[ j ];
    }
}

//7.3.2.1 Sequence parameter set RBSP syntax
void read_seq_parameter_set_rbsp(h264_stream_t* h, bs_t* b)
{
    int i;

    // NOTE can't read directly into sps because seq_parameter_set_id not yet known and so sps is not selected

    int profile_idc = bs_read_u8(b);
    int constraint_set0_flag = bs_read_u1(b);
    int constraint_set1_flag = bs_read_u1(b);
    int constraint_set2_flag = bs_read_u1(b);
    int constraint_set3_flag = bs_read_u1(b);
    int constraint_set4_flag = bs_read_u1(b);
    int constraint_set5_flag = bs_read_u1(b);
    int reserved_zero_2bits  = bs_read_u(b,2);  /* all 0's */
    int level_idc = bs_read_u8(b);
    int seq_parameter_set_id = bs_read_ue(b);

    // select the correct sps
    g_SPSTable = seq_parameter_set_id;
	#if (!_ONLY_ANALYSIS_SPS_)
    h->sps = h->sps_table[seq_parameter_set_id];
	#endif
    sps_t* sps = h->sps;
    memset(sps, 0, sizeof(sps_t));
    
    sps->chroma_format_idc = 1; 

    sps->profile_idc = profile_idc; // bs_read_u8(b);
    sps->constraint_set0_flag = constraint_set0_flag;//bs_read_u1(b);
    sps->constraint_set1_flag = constraint_set1_flag;//bs_read_u1(b);
    sps->constraint_set2_flag = constraint_set2_flag;//bs_read_u1(b);
    sps->constraint_set3_flag = constraint_set3_flag;//bs_read_u1(b);
    sps->constraint_set4_flag = constraint_set4_flag;//bs_read_u1(b);
    sps->constraint_set5_flag = constraint_set5_flag;//bs_read_u1(b);
    sps->reserved_zero_2bits = reserved_zero_2bits;//bs_read_u(b,2);
    sps->level_idc = level_idc; //bs_read_u8(b);
    sps->seq_parameter_set_id = seq_parameter_set_id; // bs_read_ue(b);
    if( sps->profile_idc == 100 || sps->profile_idc == 110 ||
        sps->profile_idc == 122 || sps->profile_idc == 144 )
    {
        sps->chroma_format_idc = bs_read_ue(b);
        if( sps->chroma_format_idc == 3 )
        {
            sps->residual_colour_transform_flag = bs_read_u1(b);
        }
        sps->bit_depth_luma_minus8 = bs_read_ue(b);
        sps->bit_depth_chroma_minus8 = bs_read_ue(b);
        sps->qpprime_y_zero_transform_bypass_flag = bs_read_u1(b);
        sps->seq_scaling_matrix_present_flag = bs_read_u1(b);
        if( sps->seq_scaling_matrix_present_flag )
        {
            for( i = 0; i < 8; i++ )
            {
                sps->seq_scaling_list_present_flag[ i ] = bs_read_u1(b);
                if( sps->seq_scaling_list_present_flag[ i ] )
                {
                    if( i < 6 )
                    {
                        read_scaling_list( b, sps->ScalingList4x4[ i ], 16,
                                      sps->UseDefaultScalingMatrix4x4Flag[ i ]);
                    }
                    else
                    {
                        read_scaling_list( b, sps->ScalingList8x8[ i - 6 ], 64,
                                      sps->UseDefaultScalingMatrix8x8Flag[ i - 6 ] );
                    }
                }
            }
        }
    }
    sps->log2_max_frame_num_minus4 = bs_read_ue(b);
    sps->pic_order_cnt_type = bs_read_ue(b);
    if( sps->pic_order_cnt_type == 0 )
    {
        sps->log2_max_pic_order_cnt_lsb_minus4 = bs_read_ue(b);
    }
    else if( sps->pic_order_cnt_type == 1 )
    {
        sps->delta_pic_order_always_zero_flag = bs_read_u1(b);
        sps->offset_for_non_ref_pic = bs_read_se(b);
        sps->offset_for_top_to_bottom_field = bs_read_se(b);
        sps->num_ref_frames_in_pic_order_cnt_cycle = bs_read_ue(b);
        for( i = 0; i < sps->num_ref_frames_in_pic_order_cnt_cycle; i++ )
        {
            sps->offset_for_ref_frame[ i ] = bs_read_se(b);
        }
    }
    sps->num_ref_frames = bs_read_ue(b);
    sps->gaps_in_frame_num_value_allowed_flag = bs_read_u1(b);
    sps->pic_width_in_mbs_minus1 = bs_read_ue(b);
    sps->pic_height_in_map_units_minus1 = bs_read_ue(b);
    sps->frame_mbs_only_flag = bs_read_u1(b);
    if( !sps->frame_mbs_only_flag )
    {
        sps->mb_adaptive_frame_field_flag = bs_read_u1(b);
    }
    sps->direct_8x8_inference_flag = bs_read_u1(b);
    sps->frame_cropping_flag = bs_read_u1(b);
    if( sps->frame_cropping_flag )
    {
        sps->frame_crop_left_offset = bs_read_ue(b);
        sps->frame_crop_right_offset = bs_read_ue(b);
        sps->frame_crop_top_offset = bs_read_ue(b);
        sps->frame_crop_bottom_offset = bs_read_ue(b);
    }
    sps->vui_parameters_present_flag = bs_read_u1(b);
    if( sps->vui_parameters_present_flag )
    {
        //read_vui_parameters(h, b);
    }
    //read_rbsp_trailing_bits(h, b);
}

/**
   Convert NAL data (Annex B format) to RBSP data.
   The size of rbsp_buf must be the same as size of the nal_buf to guarantee the output will fit.
   If that is not true, output may be truncated and an error will be returned. 
   Additionally, certain byte sequences in the input nal_buf are not allowed in the spec and also cause the conversion to fail and an error to be returned.
   @param[in] nal_buf   the nal data
   @param[in,out] nal_size  as input, pointer to the size of the nal data; as output, filled in with the actual size of the nal data
   @param[in,out] rbsp_buf   allocated memory in which to put the rbsp data
   @param[in,out] rbsp_size  as input, pointer to the maximum size of the rbsp data; as output, filled in with the actual size of rbsp data
   @return  actual size of rbsp data, or -1 on error
 */
// 7.3.1 NAL unit syntax
// 7.4.1.1 Encapsulation of an SODB within an RBSP
int nal_to_rbsp(const uint8_t* nal_buf, int* nal_size, uint8_t* rbsp_buf, int* rbsp_size)
{
    int i;
    int j     = 0;
    int count = 0;
  
    for( i = 1; i < *nal_size; i++ )
    { 
        // in NAL unit, 0x000000, 0x000001 or 0x000002 shall not occur at any byte-aligned position
        if( ( count == 2 ) && ( nal_buf[i] < 0x03) ) 
        {
            return -1;
        }

        if( ( count == 2 ) && ( nal_buf[i] == 0x03) )
        {
            // check the 4th byte after 0x000003, except when cabac_zero_word is used, in which case the last three bytes of this NAL unit must be 0x000003
            if((i < *nal_size - 1) && (nal_buf[i+1] > 0x03))
            {
                return -1;
            }

            // if cabac_zero_word is used, the final byte of this NAL unit(0x03) is discarded, and the last two bytes of RBSP must be 0x0000
            if(i == *nal_size - 1)
            {
                break;
            }

            i++;
            count = 0;
        }

        if ( j >= *rbsp_size ) 
        {
            // error, not enough space
            return -1;
        }

        rbsp_buf[j] = nal_buf[i];
        if(nal_buf[i] == 0x00)
        {
            count++;
        }
        else
        {
            count = 0;
        }
        j++;
    }

    *nal_size = i;
    *rbsp_size = j;
    return j;
}

// buf: 00 00 00 01 67 42 、、 从67开始的字符串
int read_nal_unit(h264_stream_t* h, uint8_t* buf, int size)
{
    nal_t* nal = h->nal;

    bs_t* b = bs_new(buf, size);

    nal->forbidden_zero_bit = bs_read_f(b,1);
    nal->nal_ref_idc = bs_read_u(b,2);
    nal->nal_unit_type = bs_read_u(b,5);
    nal->parsed = NULL;
    nal->sizeof_parsed = 0;

    bs_free(b);

    int nal_size = size;
    int rbsp_size = size;
    uint8_t* rbsp_buf = (uint8_t*)malloc(rbsp_size);
 
    int rc = nal_to_rbsp(buf, &nal_size, rbsp_buf, &rbsp_size);
    if (rc < 0) 
	{ 
		printf("nal_to_rbsp return error !!!\n");
		free(rbsp_buf); 
		return -1; 
	} // handle conversion error

    b = bs_new(rbsp_buf, rbsp_size);
    switch ( nal->nal_unit_type )
    {
    	case NAL_UNIT_TYPE_SPS: 
            read_seq_parameter_set_rbsp(h, b); 
            nal->parsed = h->sps;
            nal->sizeof_parsed = sizeof(sps_t);
            break;

		/*
        case NAL_UNIT_TYPE_CODED_SLICE_IDR:
        case NAL_UNIT_TYPE_CODED_SLICE_NON_IDR:  
        case NAL_UNIT_TYPE_CODED_SLICE_AUX:
            read_slice_layer_rbsp(h, b);
            nal->parsed = h->sh;
            nal->sizeof_parsed = sizeof(slice_header_t);
            break;

        case NAL_UNIT_TYPE_SEI:
            read_sei_rbsp(h, b);
            nal->parsed = h->sei;
            nal->sizeof_parsed = sizeof(sei_t);
            break;

       

        case NAL_UNIT_TYPE_PPS:   
            read_pic_parameter_set_rbsp(h, b);
            nal->parsed = h->pps;
            nal->sizeof_parsed = sizeof(pps_t);
            break;

        case NAL_UNIT_TYPE_AUD:     
            read_access_unit_delimiter_rbsp(h, b); 
            nal->parsed = h->aud;
            nal->sizeof_parsed = sizeof(aud_t);
            break;

        case NAL_UNIT_TYPE_END_OF_SEQUENCE: 
            read_end_of_seq_rbsp(h, b);
            break;

        case NAL_UNIT_TYPE_END_OF_STREAM: 
            read_end_of_stream_rbsp(h, b);
            break;
        //case NAL_UNIT_TYPE_FILLER:
        //case NAL_UNIT_TYPE_SPS_EXT:
        //case NAL_UNIT_TYPE_UNSPECIFIED:
        //case NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_A:  
        //case NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_B: 
        //case NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_C:
        */
        default:
            // here comes the reserved/unspecified/ignored stuff
            nal->parsed = NULL;
            nal->sizeof_parsed = 0;
            return 0;
    }

    if (bs_overrun(b)) 
	{ 
		bs_free(b); 
		free(rbsp_buf); 
		return -1; 
	}

    bs_free(b); 
    free(rbsp_buf);

    return nal_size;
}

sei_t* sei_new()
{
    sei_t* s = (sei_t*)malloc(sizeof(sei_t));
    memset(s, 0, sizeof(sei_t));
    s->payload = NULL;
    return s;
}

void sei_free(sei_t* s)
{
    if ( s->payload != NULL ) free(s->payload);
    free(s);
}

#if _ONLY_ANALYSIS_SPS_
/**
 Create a new H264 stream object.  Allocates all structures contained within it.
 @return    the stream object
 */
h264_stream_t* h264_new()
{
    h264_stream_t* h = (h264_stream_t*)calloc(1, sizeof(h264_stream_t));

    h->nal = (nal_t*)calloc(1, sizeof(nal_t));
    h->sps = (sps_t*)calloc(1, sizeof(sps_t));   
    return h;   
}

/**
 Free an existing H264 stream object.  Frees all contained structures.
 @param[in,out] h   the stream object
 */
void h264_free(h264_stream_t* h)
{
	if (h)
	{
		if (h->nal)
		{
			free(h->nal);
			h->nal = NULL;
		}

		if (h->sps)
		{
			free(h->sps);
			h->sps = NULL;
		}
		free(h);
		h = NULL;
	}  
}
#else
/**
 Create a new H264 stream object.  Allocates all structures contained within it.
 @return    the stream object
 */
h264_stream_t* h264_new()
{
	int i = 0;
    h264_stream_t* h = (h264_stream_t*)calloc(1, sizeof(h264_stream_t));

    h->nal = (nal_t*)calloc(1, sizeof(nal_t));

    // initialize tables
    for (i = 0; i < 32; i++ ) { h->sps_table[i] = (sps_t*)calloc(1, sizeof(sps_t)); }
    for (i = 0; i < 256; i++ ) { h->pps_table[i] = (pps_t*)calloc(1, sizeof(pps_t)); }

    h->sps = h->sps_table[0];
    h->pps = h->pps_table[0];
    h->aud = (aud_t*)calloc(1, sizeof(aud_t));
    h->num_seis = 0;
    h->seis = NULL;
    h->sei = NULL;  //This is a TEMP pointer at whats in h->seis...
    h->sh = (slice_header_t*)calloc(1, sizeof(slice_header_t));

    return h;   
}

/**
 Free an existing H264 stream object.  Frees all contained structures.
 @param[in,out] h   the stream object
 */
void h264_free(h264_stream_t* h)
{
	int i = 0;
    free(h->nal);
	h->nal = NULL;

    for (i = 0; i < 32; i++ ) 
	{
		free(h->sps_table[i]); 
		h->sps_table[i] = NULL;
		
	}
    for (i = 0; i < 256; i++ ) 
	{
		free(h->pps_table[i]);
		h->pps_table[i] = NULL;
		
	}

    free(h->aud);
	h->aud = NULL;
	
    if(h->seis != NULL)
    {
        for(i = 0; i < h->num_seis; i++ )
        {
            sei_t* sei = h->seis[i];
            sei_free(sei);
			sei = NULL;
        }
        free(h->seis);		
		h->seis = NULL;
    }
    free(h->sh);
	h->sh = NULL;
    free(h);
	h = NULL;
}
#endif

int h264_analyze(uint8_t *stream, int len)
{
	if (g_H264_Stream)
	{
		h264_free(g_H264_Stream);
		g_H264_Stream = NULL;
	}

	g_H264_Stream = h264_new();

	printf("h264_analyze  init !!!!!!!!!!!\n");
	int i = 0;
	for (i=0; i<len; i++)
	{
		printf(" %02x", stream[i]);
	}
	printf("\n\n");
	read_nal_unit(g_H264_Stream, stream, len);
}
