/* -*- c++ -*- */
/*
 * Copyright 2006,2007 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_OFDM_MAPPER_BCV_B_H
#define INCLUDED_GR_OFDM_MAPPER_BCV_B_H

#include <gr_sync_block.h>
#include <gr_message.h>
#include <gr_msg_queue.h>
#include <stdio.h>
#include <iostream>
using namespace std;

class gr_ofdm_mapper_bcv_b;
typedef boost::shared_ptr<gr_ofdm_mapper_bcv_b> gr_ofdm_mapper_bcv_b_sptr;

gr_ofdm_mapper_bcv_b_sptr 
gr_make_ofdm_mapper_bcv_b (const std::vector<gr_complex> &constellation, unsigned msgq_limit, 
			 unsigned occupied_carriers, const std::vector<gr_complex> &known_symbol,unsigned int fft_length);

/*!
 * \brief take a stream of bytes in and map to a vector of complex
 * constellation points suitable for IFFT input to be used in an ofdm
 * modulator.  Abstract class must be subclassed with specific mapping.
 * \ingroup modulation_blk
 * \ingroup ofdm_blk
 */

class gr_ofdm_mapper_bcv_b : public gr_sync_block
{
  friend gr_ofdm_mapper_bcv_b_sptr
  gr_make_ofdm_mapper_bcv_b (const std::vector<gr_complex> &constellation, unsigned msgq_limit, 
			   unsigned occupied_carriers, const std::vector<gr_complex> &known_symbol,unsigned int fft_length);
 protected:
  gr_ofdm_mapper_bcv_b (const std::vector<gr_complex> &constellation, unsigned msgq_limit, 
		      unsigned occupied_carriers, const std::vector<gr_complex> &known_symbol,unsigned int fft_length);

 private:
  std::vector<gr_complex> d_constellation;
  gr_msg_queue_sptr	d_msgq;
  gr_message_sptr	d_msg;
  unsigned		d_msg_offset;
  bool			d_eof;
  
  unsigned int 		d_occupied_carriers;
  std::vector<gr_complex> d_known_symbol;
  unsigned int 		d_fft_length;
  unsigned int 		d_bit_offset;
  int			d_pending_flag;
  int           d_pilot_flag;

  unsigned long  d_nbits;
  unsigned char  d_msgbytes;
  
  unsigned char d_resid;
  unsigned int d_nresid;

  std::vector<int> d_subcarrier_map;

  int randsym();

 public:
  ~gr_ofdm_mapper_bcv_b(void);

  gr_msg_queue_sptr	msgq() const { return d_msgq; }

  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);

};

#endif
