/*
 * MATLAB Compiler: 4.13 (R2010a)
 * Date: Wed May 11 15:11:08 2011
 * Arguments: "-B" "macro_default" "-W" "lib:mxace29_2010a" "-T" "link:lib"
 * "wav2seq" "wavletace" "freqwidth" "elimcrcr" "addmlines" 
 */

#include "mclmcrrt.h"

#ifdef __cplusplus
extern "C" {
#endif
const unsigned char __MCC_mxace29_2010a_session_key[] = {
    '1', '7', '9', '9', '5', 'D', 'A', '6', '7', 'C', '7', '8', 'D', '5', 'D',
    'A', 'D', '6', '3', '8', '3', 'E', '7', '9', '2', '6', 'E', '5', '5', 'D',
    '2', 'A', '0', '6', 'A', 'C', '2', '6', 'B', '4', '6', '9', '5', 'B', 'E',
    '2', '8', '9', '0', '5', '0', 'A', 'E', 'F', '4', 'F', '3', '9', 'A', '1',
    'A', '8', '1', '9', '2', '9', '4', '6', '8', 'E', 'B', 'B', '7', '7', '5',
    'B', '6', '2', '2', 'C', '7', '7', '2', '4', '2', 'A', 'F', '5', '2', '2',
    'A', 'D', '8', 'E', '5', '0', '8', '3', 'C', '3', 'A', '9', '4', '4', 'F',
    '7', '1', '8', '1', 'E', '3', '9', 'C', '5', '5', 'E', 'D', '5', 'C', 'D',
    '8', 'F', '3', '8', '8', '6', '6', '6', '7', '3', '0', 'A', '4', 'C', '8',
    'E', 'F', 'F', '1', 'B', '3', '9', '8', '6', 'A', '3', '5', '0', '2', '7',
    '6', '3', '6', 'F', 'C', '3', '2', 'F', '4', 'C', '2', 'B', '8', 'F', 'D',
    '2', '0', '3', '5', '7', '1', 'A', '8', '3', '9', 'E', '7', 'C', 'C', '7',
    'B', '7', 'A', '5', '0', 'E', 'A', 'D', '2', '3', '7', '9', '1', 'E', '0',
    '7', 'E', 'A', '0', '3', '4', '9', 'A', 'F', '1', '6', 'C', '3', 'A', '1',
    '5', 'F', '6', 'B', 'E', 'B', 'B', 'D', '0', 'A', '1', '3', '2', '1', '7',
    '9', 'D', 'B', 'D', '8', 'A', 'B', '1', 'A', '5', '7', '1', '5', '3', '9',
    '0', '5', '7', 'C', '8', '2', 'D', '9', '3', '5', 'E', 'F', '6', 'C', '9',
    '5', '\0'};

const unsigned char __MCC_mxace29_2010a_public_key[] = {
    '3', '0', '8', '1', '9', 'D', '3', '0', '0', 'D', '0', '6', '0', '9', '2',
    'A', '8', '6', '4', '8', '8', '6', 'F', '7', '0', 'D', '0', '1', '0', '1',
    '0', '1', '0', '5', '0', '0', '0', '3', '8', '1', '8', 'B', '0', '0', '3',
    '0', '8', '1', '8', '7', '0', '2', '8', '1', '8', '1', '0', '0', 'C', '4',
    '9', 'C', 'A', 'C', '3', '4', 'E', 'D', '1', '3', 'A', '5', '2', '0', '6',
    '5', '8', 'F', '6', 'F', '8', 'E', '0', '1', '3', '8', 'C', '4', '3', '1',
    '5', 'B', '4', '3', '1', '5', '2', '7', '7', 'E', 'D', '3', 'F', '7', 'D',
    'A', 'E', '5', '3', '0', '9', '9', 'D', 'B', '0', '8', 'E', 'E', '5', '8',
    '9', 'F', '8', '0', '4', 'D', '4', 'B', '9', '8', '1', '3', '2', '6', 'A',
    '5', '2', 'C', 'C', 'E', '4', '3', '8', '2', 'E', '9', 'F', '2', 'B', '4',
    'D', '0', '8', '5', 'E', 'B', '9', '5', '0', 'C', '7', 'A', 'B', '1', '2',
    'E', 'D', 'E', '2', 'D', '4', '1', '2', '9', '7', '8', '2', '0', 'E', '6',
    '3', '7', '7', 'A', '5', 'F', 'E', 'B', '5', '6', '8', '9', 'D', '4', 'E',
    '6', '0', '3', '2', 'F', '6', '0', 'C', '4', '3', '0', '7', '4', 'A', '0',
    '4', 'C', '2', '6', 'A', 'B', '7', '2', 'F', '5', '4', 'B', '5', '1', 'B',
    'B', '4', '6', '0', '5', '7', '8', '7', '8', '5', 'B', '1', '9', '9', '0',
    '1', '4', '3', '1', '4', 'A', '6', '5', 'F', '0', '9', '0', 'B', '6', '1',
    'F', 'C', '2', '0', '1', '6', '9', '4', '5', '3', 'B', '5', '8', 'F', 'C',
    '8', 'B', 'A', '4', '3', 'E', '6', '7', '7', '6', 'E', 'B', '7', 'E', 'C',
    'D', '3', '1', '7', '8', 'B', '5', '6', 'A', 'B', '0', 'F', 'A', '0', '6',
    'D', 'D', '6', '4', '9', '6', '7', 'C', 'B', '1', '4', '9', 'E', '5', '0',
    '2', '0', '1', '1', '1', '\0'};

static const char * MCC_mxace29_2010a_matlabpath_data[] = 
  { "mxace29_2010/", "$TOOLBOXDEPLOYDIR/", "matlab/work/",
    "nic/4.20/MATLAB/FTM/", "nic/4.20/MATLAB/Filterbank/",
    "nic/4.20/MATLAB/FrontEnd/", "nic/4.20/MATLAB/Implant/",
    "nic/4.20/MATLAB/Processing/", "nic/4.20/MATLAB/Sequence/",
    "nic/4.20/MATLAB/Strategy/", "nic/4.20/MATLAB/Utility/",
    "nic/2/matlab/m_files/", "nic/2/binaries/",
    "$TOOLBOXMATLABDIR/general/", "$TOOLBOXMATLABDIR/ops/",
    "$TOOLBOXMATLABDIR/lang/", "$TOOLBOXMATLABDIR/elmat/",
    "$TOOLBOXMATLABDIR/randfun/", "$TOOLBOXMATLABDIR/elfun/",
    "$TOOLBOXMATLABDIR/specfun/", "$TOOLBOXMATLABDIR/matfun/",
    "$TOOLBOXMATLABDIR/datafun/", "$TOOLBOXMATLABDIR/polyfun/",
    "$TOOLBOXMATLABDIR/funfun/", "$TOOLBOXMATLABDIR/sparfun/",
    "$TOOLBOXMATLABDIR/scribe/", "$TOOLBOXMATLABDIR/graph2d/",
    "$TOOLBOXMATLABDIR/graph3d/", "$TOOLBOXMATLABDIR/specgraph/",
    "$TOOLBOXMATLABDIR/graphics/", "$TOOLBOXMATLABDIR/uitools/",
    "$TOOLBOXMATLABDIR/strfun/", "$TOOLBOXMATLABDIR/imagesci/",
    "$TOOLBOXMATLABDIR/iofun/", "$TOOLBOXMATLABDIR/audiovideo/",
    "$TOOLBOXMATLABDIR/timefun/", "$TOOLBOXMATLABDIR/datatypes/",
    "$TOOLBOXMATLABDIR/verctrl/", "$TOOLBOXMATLABDIR/codetools/",
    "$TOOLBOXMATLABDIR/helptools/", "$TOOLBOXMATLABDIR/winfun/",
    "$TOOLBOXMATLABDIR/winfun/net/", "$TOOLBOXMATLABDIR/demos/",
    "$TOOLBOXMATLABDIR/timeseries/", "$TOOLBOXMATLABDIR/hds/",
    "$TOOLBOXMATLABDIR/guide/", "$TOOLBOXMATLABDIR/plottools/",
    "toolbox/local/", "$TOOLBOXMATLABDIR/datamanager/",
    "toolbox/compiler/", "toolbox/signal/signal/",
    "toolbox/signal/sigtools/", "toolbox/shared/spcuilib/" };

static const char * MCC_mxace29_2010a_classpath_data[] = 
  { "" };

static const char * MCC_mxace29_2010a_libpath_data[] = 
  { "" };

static const char * MCC_mxace29_2010a_app_opts_data[] = 
  { "" };

static const char * MCC_mxace29_2010a_run_opts_data[] = 
  { "" };

static const char * MCC_mxace29_2010a_warning_state_data[] = 
  { "off:MATLAB:dispatcher:nameConflict" };


mclComponentData __MCC_mxace29_2010a_component_data = { 

  /* Public key data */
  __MCC_mxace29_2010a_public_key,

  /* Component name */
  "mxace29_2010a",

  /* Component Root */
  "",

  /* Application key data */
  __MCC_mxace29_2010a_session_key,

  /* Component's MATLAB Path */
  MCC_mxace29_2010a_matlabpath_data,

  /* Number of directories in the MATLAB Path */
  53,

  /* Component's Java class path */
  MCC_mxace29_2010a_classpath_data,
  /* Number of directories in the Java class path */
  0,

  /* Component's load library path (for extra shared libraries) */
  MCC_mxace29_2010a_libpath_data,
  /* Number of directories in the load library path */
  0,

  /* MCR instance-specific runtime options */
  MCC_mxace29_2010a_app_opts_data,
  /* Number of MCR instance-specific runtime options */
  0,

  /* MCR global runtime options */
  MCC_mxace29_2010a_run_opts_data,
  /* Number of MCR global runtime options */
  0,
  
  /* Component preferences directory */
  "mxace29_2010_7C89DBA6221DE40A4ADB237720A4DC93",

  /* MCR warning status data */
  MCC_mxace29_2010a_warning_state_data,
  /* Number of MCR warning status modifiers */
  1,

  /* Path to component - evaluated at runtime */
  NULL

};

#ifdef __cplusplus
}
#endif


