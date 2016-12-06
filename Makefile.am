##########################################################################
# If not stated otherwise in this file or this component's Licenses.txt
# file the following copyright and licenses apply:
#
# Copyright 2016 RDK Management
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
##########################################################################
SUBDIRS = device_settings_utils trm_diag_utils
DIST_SUBDIRS = device_settings_utils trm_diag_utils

## IPK generation Support
sys_utils_IPK_OUTDIR = $(abs_top_builddir)/ipk
sys_utils_IPK_STAGING_DIR=$(abs_top_builddir)/staging_dir
sys_utils_IPK_TMP_DIR=$(sys_utils_IPK_STAGING_DIR)/ipk

sys_utils_CONTROL_FILES = control
sys_utils_IPK_FILES  = debian-binary control.tar.gz data.tar.gz
sys_utils_IPK_OUTPUT = sys_utils.ipk

WRKDIR          =       .
SYS_USR_DIR     =       /usr

package_ipk:
	echo "Generating IPK Package"
	install -d $(sys_utils_IPK_STAGING_DIR)$(SYS_USR_DIR)/bin
	install -m 0755 $(WRKDIR)/QueryDSState $(sys_utils_IPK_STAGING_DIR)$(SYS_USR_DIR)/bin
	tar -czvf $(sys_utils_IPK_OUTDIR)/data.tar.gz -C $(sys_utils_IPK_STAGING_DIR) .
	tar -czvf $(sys_utils_IPK_OUTDIR)/control.tar.gz -C $(sys_utils_IPK_OUTDIR) $(sys_utils_CONTROL_FILES)
	cd $(sys_utils_IPK_OUTDIR) && ar cr $(sys_utils_IPK_OUTPUT) $(sys_utils_IPK_FILES) && cd -
	rm -rf $(sys_utils_IPK_STAGING_DIR)
