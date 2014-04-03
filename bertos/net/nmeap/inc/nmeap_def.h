/*
Copyright (c) 2005, David M Howard (daveh at dmh2000.com)
All rights reserved.

This product is licensed for use and distribution under the BSD Open Source License.
see the file COPYING for more details.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#ifndef __NMEAP_DEF_H__
#define __NMEAP_DEF_H__

/**
 * context for a single sentence
 */
typedef struct nmeap_sentence {
    char                    name[NMEAP_MAX_SENTENCE_NAME_LENGTH + 1];
	int                     id;
    nmeap_sentence_parser_t parser;
    nmeap_callout_t         callout;
    void                   *data;
} nmeap_sentence_t;

/**
 * parser context
 */
struct nmeap_context {
	/** support up to 8 sentences */
	nmeap_sentence_t sentence[NMEAP_MAX_SENTENCES];		/* sentence descriptors */
	int              sentence_count;						/* number of initialized descriptors */

	/** sentence input buffer */
	char             input[NMEAP_MAX_SENTENCE_LENGTH + 1];	 /* input line buffer */
	int              input_count;	                        /* index into 'input */
	int              input_state;	                        /* current lexical scanner state */
	char             input_name[6];                        /* sentence name */
	char             icks;			                        /* input checksum    */
	char             ccks;			                        /* computed checksum */

	/* tokenization */
	char            *token[NMEAP_MAX_TOKENS];              /* list of delimited tokens */
	int              tokens;							     /* list of tokens */

	/** errors and debug. optimize these as desired */
	unsigned long    msgs;    /* count of good messages */
	unsigned long    err_hdr; /* header error */
	unsigned long    err_ovr; /* overrun error */
	unsigned long    err_unk; /* unknown error */
	unsigned long    err_id;  /* bad character in id */
	unsigned long    err_cks; /* bad checksum */
	unsigned long    err_crl; /* expecting cr or lf, got something else */
	char             debug_input[NMEAP_MAX_SENTENCE_LENGTH + 1];	 /* input line buffer for debug */

	/** opaque user data */
	void *user_data;
};

typedef struct nmeap_context nmeap_context_t;

#endif /* __NMEAP_DEF_H__ */
