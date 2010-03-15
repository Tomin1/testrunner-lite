/* * This file is part of testrunner-lite *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 *
 * Contact: Sampo Saaristo <ext-sampo.1.saaristo@nokia.com>
 *
 * This software, including documentation, is protected by copyright
 * controlled by Nokia Corporation. All rights are reserved. Copying,
 * including reproducing, storing, adapting or translating, any or all of
 * this material requires the prior written consent of Nokia Corporation.
 * This material also contains confidential information which may not be
 * disclosed to others without the prior written consent of Nokia.
 *
 */

/* ------------------------------------------------------------------------- */
/* INCLUDE FILES */
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>

#include <libxml/xmlreader.h>
#include <libxml/xmlschemas.h>

#include "testrunnerlite.h"
#include "testdefinitiondatatypes.h"
#include "log.h"

/* ------------------------------------------------------------------------- */
/* EXTERNAL DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* EXTERNAL FUNCTION PROTOTYPES */
/* None */

/* ------------------------------------------------------------------------- */
/* GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* CONSTANTS */
/* None */

/* ------------------------------------------------------------------------- */
/* MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL GLOBAL VARIABLES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL CONSTANTS AND MACROS */
/* None */

/* ------------------------------------------------------------------------- */
/* MODULE DATA STRUCTURES */
/* None */

/* ------------------------------------------------------------------------- */
/* LOCAL FUNCTION PROTOTYPES */
/* ------------------------------------------------------------------------- */
LOCAL void list_string_delete (xmlLinkPtr);
/* ------------------------------------------------------------------------- */
LOCAL void gen_attribs_delete (td_gen_attribs *);
/* ------------------------------------------------------------------------- */
LOCAL int list_string_compare (const void *, const void *);
/* ------------------------------------------------------------------------- */
/* FORWARD DECLARATIONS */
/* None */

/* ------------------------------------------------------------------------- */
/* ==================== LOCAL FUNCTIONS ==================================== */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/** Deallocator for list with xmlchar* items
 *  @param lk list item
 */
LOCAL void list_string_delete (xmlLinkPtr lk)
{
	xmlChar *string = (xmlChar *)xmlLinkGetData (lk);
	free (string);
}
/* ------------------------------------------------------------------------- */
/** Comparison function for list with xmlchar* items
 *  @param data0 string to compare
 *  @param data1 string to compare
 *  @return 0 if strings match 
 */
LOCAL int list_string_compare(const void * data0, 
			      const void * data1)
{
	
	return xmlStrcmp ((xmlChar *)data0, (xmlChar *)data1);
}
/* ------------------------------------------------------------------------- */
/** Comparison function for list without ordering
 *  @param data0 string to compare - not used
 *  @param data1 string to compare - not used
 *  @return 0 always
 */
LOCAL int list_dummy_compare(const void * data0, 
			      const void * data1)
{
	
	return 0;
}
/* ------------------------------------------------------------------------- */
/** Deallocator for general attributes 
 *  @param gen general attributes 
 */
LOCAL void gen_attribs_delete (td_gen_attribs *gen)
{
	free (gen->name);
	free (gen->description);
	free (gen->requirement);
	free (gen->level);
}
/* ------------------------------------------------------------------------- */
/* ======================== FUNCTIONS ====================================== */
/* ------------------------------------------------------------------------- */
/** Creates a td_suite data structure, initializes lists for pre_steps etc.
 *  @return pointer to td_set or NULL in case of OOM
 */
td_suite *td_suite_create()
{
	td_suite *s = (td_suite *)malloc (sizeof (td_suite));
	if (s == NULL) {
		log_msg (LOG_ERROR, "%s: FATAL : OOM", PROGNAME);
		return NULL;
	}

	memset (s, 0x0, sizeof (td_suite));
	return s;
}
/* ------------------------------------------------------------------------- */
/** De-allocate td_suite data structure
 *  @param *s td_suite data 
 */
void td_suite_delete(td_suite *s)
{
	gen_attribs_delete (&s->gen);
	if (s->domain) free (s->domain);
#if 0
	if (s->suite_type) free (s->suite_type);
#endif
	free (s);
}
/* ------------------------------------------------------------------------- */
/** Creates a td_set data structure, initializes lists for pre_steps etc.
 *  @return pointer to td_set or NULL in case of OOM
 */
td_set *td_set_create ()
{
	td_set *set = (td_set *)malloc (sizeof (td_set));
	if (set == NULL) {
		log_msg (LOG_ERROR, "%s: FATAL : OOM", PROGNAME);
		return NULL;
	}
	memset (set, 0x0, sizeof (td_set));
	set->pre_steps = xmlListCreate (td_step_delete, list_dummy_compare);
	set->post_steps = xmlListCreate (td_step_delete, list_dummy_compare);
	set->cases = xmlListCreate (td_case_delete, list_dummy_compare);
	set->environments = xmlListCreate (list_string_delete, 
					   list_string_compare);
	set->gets = xmlListCreate (list_string_delete, NULL);

	return set;
}
/* ------------------------------------------------------------------------- */
/** De-allocate td_set data structure
 *  @param *s td_set data 
 */
void td_set_delete(td_set *s)
{
	gen_attribs_delete(&s->gen);
	free (s->feature);
	
	xmlListDelete (s->pre_steps);
	xmlListDelete (s->post_steps);
	xmlListDelete (s->cases);
	xmlListDelete (s->environments);
	xmlListDelete (s->gets);
	free (s->environment);
	free (s);
}
/* ------------------------------------------------------------------------- */
/** Creates a td_step data structure
 *  @return pointer to td_step or NULL in case of OOM
 */
td_step *td_step_create()
{
	td_step *step;

	step = (td_step *) malloc (sizeof (td_step));
	if (step == NULL) {
		log_msg (LOG_ERROR, "%s: FATAL : OOM", PROGNAME);
		return NULL;
	}
	memset (step, 0x0, sizeof (td_step));
	return step;
}
/* ------------------------------------------------------------------------- */
/** Creates a td_case data structure
 *  @return pointer to td_case or NULL in case of OOM
 */
td_case *td_case_create()
{
	td_case *td_c;

	td_c = (td_case *) malloc (sizeof (td_case));
	if (td_c == NULL) {
		log_msg (LOG_ERROR, "%s: FATAL : OOM", PROGNAME);
		return NULL;
	}
	memset (td_c, 0x0, sizeof (td_case));
	td_c->steps = xmlListCreate (td_step_delete, list_dummy_compare);

	return td_c;
}
/* ------------------------------------------------------------------------- */
/** Deallocator for td_step called by xmlListDelete
 */
void td_step_delete(xmlLinkPtr lk)
{
	td_step *step = xmlLinkGetData (lk);
	if (step->step)
		free (step->step);

	free (step->stdout_);
	free (step->stderr_);
	free (step->failure_info);
	
	free (step);
}
/* ------------------------------------------------------------------------- */
/** Deallocator for  td_case data structure
 */
void td_case_delete(xmlLinkPtr lk)
{
	td_case *td_c = xmlLinkGetData (lk);
	xmlListDelete (td_c->steps);
	
	gen_attribs_delete(&td_c->gen);
	free (td_c);
}

/* ================= OTHER EXPORTED FUNCTIONS ============================== */
/* None */

/* ------------------------------------------------------------------------- */
/* End of file */

