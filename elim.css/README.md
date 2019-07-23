# UNIX/Linux Context Switches per Second ELIM for LSF

## Description

The ELIM included in this directory reports an hosts context switches per second to LSF.  This data can be used visually track host context switches in LSF RTM via it's ELIM Template graphs.  

## Use Cases

The context switches ELIM can be used for the following purposes:

1) Reporting in tools like RTM using ELIM Templates

2) Sorting of devices to be used for dispatch, or to select hosts with a specific number of context switches per second range.  

For example, you can do the following:

	bsub -R "order[css]" ./a.out

Which would dispatch to the host with the lowest number of context switches.  Additionally, you could do the following:

	bsub -R "select[css<1000]" ./a.out

The bsub above would only select hosts that have an css of less than 1000 context switches per second to dispatch to.

## Installation Instructions

To install this ELIM, you must first add the "css" numeric resource to LSF as per the normal process which involves updating your lsf.shared and lsf.cluster files to include the value.  Ensure that you assign this resource to hosts your UNIX/Linux hosts.

Then, before restarting the cluster, make sure that the elim.css binary has been copied to the $LSF_SERVERDIR for all compute hosts and marked executable.  After which, you can restart your cluster using:

	lsadmin reconfig
	badmin reconfig

Make sure you restart all LIM's and not just the Master LIM.  From each compute host, you should then see the binary running in the background.  If not, you should debug the binary interactively using simply by running from the command line on your hosts.

You can also get the context switches per second of your hosts by running the following lsload command:

	lsload -o "css"

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

