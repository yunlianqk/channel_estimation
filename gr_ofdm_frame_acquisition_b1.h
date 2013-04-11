/* -*- c++ -*- */
/*
 * Copyright 2006, 2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_OFDM_FRAME_ACQUISITION_B1_H
#define INCLUDED_GR_OFDM_FRAME_ACQUISITION_B1_H


#include <gr_block.h>
#include <vector>

class gr_ofdm_frame_acquisition_b1;
typedef boost::shared_ptr<gr_ofdm_frame_acquisition_b1> gr_ofdm_frame_acquisition_b1_sptr;

gr_ofdm_frame_acquisition_b1_sptr 
gr_make_ofdm_frame_acquisition_b1 (unsigned int occupied_carriers, unsigned int fft_length,
				unsigned int cplen,
				const std::vector<gr_complex> &known_symbol, 
				const std::vector<gr_complex> &known_pilot,
				unsigned int max_fft_shift_len=10);

/*!
 * \brief take a vector of complex constellation points in from an FFT
 * and performs a correlation and equalization.
 * \ingroup demodulation_blk
 * \ingroup ofdm_blk
 *
 * This block takes the output of an FFT of a received OFDM symbol and finds the 
 * start of a frame based on two known symbols. It also looks at the surrounding
 * bins in the FFT output for the correlation in case there is a large frequency
 * shift in the data. This block assumes that the fine frequency shift has already
 * been corrected and that the samples fall in the middle of one FFT bin.
 *
 * It then uses one of those known
 * symbols to estimate the channel response over all subcarriers and does a simple 
 * 1-tap equalization on all subcarriers. This corrects for the phase and amplitude
 * distortion caused by the channel.
 */

class gr_ofdm_frame_acquisition_b1 : public gr_block
{
  /*! 
   * \brief Build an OFDM correlator and equalizer.
   * \param occupied_carriers   The number of subcarriers with data in the received symbol
   * \param fft_length          The size of the FFT vector (occupied_carriers + unused carriers)
   * \param cplen		The length of the cycle prefix
   * \param known_symbol        A vector of complex numbers representing a known symbol at the
   *                            start of a frame (usually a BPSK PN sequence)
   * \param max_fft_shift_len   Set's the maximum distance you can look between bins for correlation
   */
  friend gr_ofdm_frame_acquisition_b1_sptr
  gr_make_ofdm_frame_acquisition_b1 (unsigned int occupied_carriers, unsigned int fft_length,
				  unsigned int cplen,
				  const std::vector<gr_complex> &known_symbol, 
				  const std::vector<gr_complex> &known_pilot, 
				  unsigned int max_fft_shift_len);
  
protected:
  gr_ofdm_frame_acquisition_b1 (unsigned int occupied_carriers, unsigned int fft_length,
			     unsigned int cplen,
			     const std::vector<gr_complex> &known_symbol,
				 const std::vector<gr_complex> &known_pilot,
			     unsigned int max_fft_shift_len);
  
 private:
  unsigned char slicer(gr_complex x);
  void correlate(const gr_complex *symbol, int zeros_on_left);
  void calculate_equalizer(const gr_complex *symbol, int zeros_on_left);
  void calculate_estimation(const gr_complex *symbol,unsigned int d_phase_count,int zeros_on_left);
  gr_complex coarse_freq_comp(int freq_delta, int count);
  
  unsigned int d_occupied_carriers;  // !< \brief number of subcarriers with data
  unsigned int d_fft_length;         // !< \brief length of FFT vector
  unsigned int d_cplen;              // !< \brief length of cyclic prefix in samples
  unsigned int d_freq_shift_len;     // !< \brief number of surrounding bins to look at for correlation
  std::vector<gr_complex> d_known_symbol; // !< \brief known symbols at start of frame
  std::vector<gr_complex> d_known_pilot; // !< \brief known pilot at start of a packet of data
  std::vector<float> d_known_phase_diff; // !< \brief factor used in correlation from known symbol
  std::vector<float> d_symbol_phase_diff; // !< \brief factor used in correlation from received symbol
  std::vector<gr_complex> d_hestimate;  // !< preamble estimate
  std::vector<gr_complex> d_pilot;  // !< channel estimate
  int d_coarse_freq;             // !< \brief search distance in number of bins
  unsigned int d_phase_count;           // !< \brief accumulator for coarse freq correction
  unsigned int d_pilot_flag;
  float d_snr_est;                      // !< an estimation of the signal to noise ratio

  gr_complex *d_phase_lut;  // !< look-up table for coarse frequency compensation
  
  std::vector<int> d_subcarrier_map;

  void forecast(int noutput_items, gr_vector_int &ninput_items_required);

 public:
  /*!
   * \brief Return an estimate of the SNR of the channel
   */
  float snr() { return d_snr_est; }

  ~gr_ofdm_frame_acquisition_b1(void);
  int general_work(int noutput_items,
		   gr_vector_int &ninput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items);
};


#endif
