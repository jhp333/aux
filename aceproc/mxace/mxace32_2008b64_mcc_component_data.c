/*
 * MATLAB Compiler: 4.9 (R2008b)
 * Date: Sun Feb 02 23:24:00 2014
 * Arguments: "-B" "macro_default" "-W" "lib:mxace32_2008b64" "-T" "link:lib"
 * "wav2seq" "wavletace" "freqwidth" "turn2cic4" "show_p" 
 */

#include "mclmcrrt.h"

#ifdef __cplusplus
extern "C" {
#endif
const unsigned char __MCC_mxace32_2008b64_session_key[] = {
    '3', '2', 'B', 'A', 'F', '2', 'C', 'C', 'F', 'A', '7', '5', 'C', 'C', 'B',
    '1', '8', 'B', 'E', 'D', '8', '8', '5', '5', '5', '4', '7', '4', '2', '8',
    'D', '1', '6', 'B', '6', '2', 'B', '4', '4', '1', 'F', '2', 'A', '4', '7',
    '9', '2', '2', '0', 'B', 'B', '2', 'A', 'B', '6', '1', '8', 'B', '5', 'D',
    '9', '7', '3', 'F', 'F', 'F', 'D', '3', '0', 'D', '0', '8', '0', 'D', '3',
    '4', 'D', '9', '8', '1', '8', '1', '0', '2', 'A', 'D', 'A', '3', 'D', 'D',
    '9', '1', '4', '4', '1', '6', 'D', '8', '2', '1', 'F', '2', '5', '0', '2',
    '7', '5', '2', '2', '7', 'B', 'C', 'F', 'E', '5', '8', 'A', '7', '0', 'B',
    '3', '5', 'B', 'F', '9', 'F', 'D', 'F', '2', '0', '0', '4', 'E', '5', '0',
    'F', 'A', 'A', '6', 'A', 'E', 'D', 'B', '6', '2', '6', '5', 'F', '5', '6',
    '3', '0', 'B', 'C', '4', '7', 'E', '5', '7', '4', '6', '1', '7', '0', 'C',
    '8', '8', 'B', 'B', '6', '1', '6', 'F', '9', '3', 'F', 'D', 'B', '7', '3',
    'D', 'B', '5', '9', 'E', 'A', '4', '3', '5', '2', 'E', '5', '2', '4', 'B',
    '2', 'C', '0', '7', '3', 'E', '7', '0', 'F', 'D', 'B', '8', '5', '0', '8',
    'B', '0', '6', 'E', '3', '0', '6', '5', '9', '1', 'F', '7', 'A', 'F', '7',
    'D', '5', 'D', '9', 'A', '7', '8', 'C', 'F', '3', '0', '4', 'F', 'D', '9',
    '9', 'E', 'E', '9', 'F', '4', '4', '9', 'E', '5', '9', 'B', 'C', 'B', '2',
    '4', '\0'};

const unsigned char __MCC_mxace32_2008b64_public_key[] = {
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

static const char * MCC_mxace32_2008b64_matlabpath_data[] = 
  { "mxace32_2008/", "$TOOLBOXDEPLOYDIR/", "NIC/4.20/MATLAB/FTM/",
    "NIC/4.20/MATLAB/Filterbank/", "NIC/4.20/MATLAB/FrontEnd/",
    "NIC/4.20/MATLAB/Implant/", "NIC/4.20/MATLAB/LoudnessGrowth/",
    "NIC/4.20/MATLAB/Processing/", "NIC/4.20/MATLAB/Sequence/",
    "NIC/4.20/MATLAB/Strategy/", "NIC/4.20/MATLAB/Utility/",
    "NIC/2/matlab/m_files/", "$TOOLBOXMATLABDIR/general/",
    "$TOOLBOXMATLABDIR/ops/", "$TOOLBOXMATLABDIR/lang/",
    "$TOOLBOXMATLABDIR/elmat/", "$TOOLBOXMATLABDIR/randfun/",
    "$TOOLBOXMATLABDIR/elfun/", "$TOOLBOXMATLABDIR/specfun/",
    "$TOOLBOXMATLABDIR/matfun/", "$TOOLBOXMATLABDIR/datafun/",
    "$TOOLBOXMATLABDIR/polyfun/", "$TOOLBOXMATLABDIR/funfun/",
    "$TOOLBOXMATLABDIR/sparfun/", "$TOOLBOXMATLABDIR/scribe/",
    "$TOOLBOXMATLABDIR/graph2d/", "$TOOLBOXMATLABDIR/graph3d/",
    "$TOOLBOXMATLABDIR/specgraph/", "$TOOLBOXMATLABDIR/graphics/",
    "$TOOLBOXMATLABDIR/uitools/", "$TOOLBOXMATLABDIR/strfun/",
    "$TOOLBOXMATLABDIR/imagesci/", "$TOOLBOXMATLABDIR/iofun/",
    "$TOOLBOXMATLABDIR/audiovideo/", "$TOOLBOXMATLABDIR/timefun/",
    "$TOOLBOXMATLABDIR/datatypes/", "$TOOLBOXMATLABDIR/verctrl/",
    "$TOOLBOXMATLABDIR/codetools/",
    "$TOOLBOXMATLABDIR/helptools/", "$TOOLBOXMATLABDIR/winfun/",
    "$TOOLBOXMATLABDIR/demos/", "$TOOLBOXMATLABDIR/timeseries/",
    "$TOOLBOXMATLABDIR/hds/", "$TOOLBOXMATLABDIR/guide/",
    "$TOOLBOXMATLABDIR/plottools/", "toolbox/local/",
    "toolbox/shared/dastudio/", "$TOOLBOXMATLABDIR/datamanager/",
    "toolbox/compiler/", "toolbox/shared/spcuilib/",
    "toolbox/signal/signal/", "toolbox/signal/sigtools/" };

static const char * MCC_mxace32_2008b64_classpath_data[] = 
  { "" };

static const char * MCC_mxace32_2008b64_libpath_data[] = 
  { "" };

static const char * MCC_mxace32_2008b64_app_opts_data[] = 
  { "" };

static const char * MCC_mxace32_2008b64_run_opts_data[] = 
  { "" };

static const char * MCC_mxace32_2008b64_warning_state_data[] = 
  { "off:MATLAB:dispatcher:nameConflict" };


mclComponentData __MCC_mxace32_2008b64_component_data = { 

  /* Public key data */
  __MCC_mxace32_2008b64_public_key,

  /* Component name */
  "mxace32_2008b64",

  /* Component Root */
  "",

  /* Application key data */
  __MCC_mxace32_2008b64_session_key,

  /* Component's MATLAB Path */
  MCC_mxace32_2008b64_matlabpath_data,

  /* Number of directories in the MATLAB Path */
  52,

  /* Component's Java class path */
  MCC_mxace32_2008b64_classpath_data,
  /* Number of directories in the Java class path */
  0,

  /* Component's load library path (for extra shared libraries) */
  MCC_mxace32_2008b64_libpath_data,
  /* Number of directories in the load library path */
  0,

  /* MCR instance-specific runtime options */
  MCC_mxace32_2008b64_app_opts_data,
  /* Number of MCR instance-specific runtime options */
  0,

  /* MCR global runtime options */
  MCC_mxace32_2008b64_run_opts_data,
  /* Number of MCR global runtime options */
  0,
  
  /* Component preferences directory */
  "mxace32_2008_496F410956812F0AD0AC4EA199758BE5",

  /* MCR warning status data */
  MCC_mxace32_2008b64_warning_state_data,
  /* Number of MCR warning status modifiers */
  1,

  /* Path to component - evaluated at runtime */
  NULL

};

#ifdef __cplusplus
}
#endif


