# UNIX/Linux Uptime ELIMs for LSF

## Description

The elims included in this directory are examples for reporting a hosts uptime to LSF.  This data can be used to steer dispatch to a specific host, or to visually track host reboots through the creation of LSF RTM ELIM Template graphs.  

## Use Cases

The uptime ELIM can be used for the following purposes:

1) Reporting in tools like RTM using ELIM Templates

2) Sorting of devices to be used for dispatch, or to select hosts with a specific uptime range.  

For example, you can do the following:

	bsub -R "order[uptime]" ./a.out

Which would dispatch to the host with the lowest uptime.  Additionally, you could do the following:

	bsub -R "select[uptime>86400]" ./a.out

The bsub above would only select hosts that have an uptime of greater than a day to dispatch to.

3) For use in custom RTM plugins, for example the gridhhist Cacti Plugin that reports uptime and other critical long term host metrics in RTM.


## Installation Instructions

To install this elim, you must first add the "uptime" numeric resource to LSF as per the normal process which involves updating your lsf.shared and lsf.cluster files to include the value.  Ensure that you assign this resource to hosts your UNIX/Linux hosts.

Then, before restarting the cluster, make sure that the elim.uptime or elim.uptime.pl binary has been copied to the $LSF_SERVERDIR for all compute hosts and marked executable.  After which, you can restart your cluster using:

	lsadmin reconfig
	badmin reconfig

Make sure you restart all LIM's and not just the Master LIM.  From each compute host, you should then see the binary running in the background.  If not, you should debug the binary interactively using simply by running from the command line on your hosts.

You can also get uptime of your hosts by running the following lsload command:

	lsload -o "uptime"

## Community Contribution Requirements

Community contributions to this repository must follow the [IBM Developer's Certificate of Origin (DCO)](https://github.com/IBMSpectrumComputing/platform-python-lsf-api/blob/master/IBMDCO.md) process and only through GitHub Pull Requests:

 1. Contributor proposes new code to community.

 2. Contributor signs off on contributions 
    (i.e. attachs the DCO to ensure contributor is either the code 
    originator or has rights to publish. The template of the DCO is included in
    this package).
 
 3. IBM Spectrum LSF development reviews contribution to check for:
    i)  Applicability and relevancy of functional content 
    ii) Any obvious issues

 4. If accepted, posts contribution. If rejected, work goes back to contributor and is not merged.

## Copyright

(C) Copyright IBM Corporation 2016-2019

U.S. Government Users Restricted Rights - Use, duplication or disclosure 
restricted by GSA ADP Schedule Contract with IBM Corp.

IBM(R), the IBM logo and ibm.com(R) are trademarks of International Business Machines Corp., 
registered in many jurisdictions worldwide. Other product and service names might be trademarks 
of IBM or other companies. A current list of IBM trademarks is available on the Web at 
"Copyright and trademark information" at [IBM Legal](www.ibm.com/legal/copytrade.shtml).

