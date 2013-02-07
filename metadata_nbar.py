#!/auto/xrut/sw/xrut-cel-5/bin/python
#
# January 2013, adikumar@cisco.com
#
# Copyright (c) 2012-2013 by Cisco Systems, Inc.
# All rights reserved.
#
#

#
# 1. Standard Preamble
#
import sys, os, time
os.environ['PY_XRUT_ROOT'] = os.path.dirname(os.path.dirname(os.path.abspath(sys.argv[0])))
sys.path[1:1] = [os.environ.get('PY_XRUT_ROOT') + '/modules']

#
# 2. Import modules we need.
#
import xrut, pre, utng, metadata, pdb
from xrut import routers        # namespace exposure
from mcast import pagent as pmc

#
# 3. Create the topology.
#
#   [ iol1 ] -------- [ iol2 ] ------ [ iol3 ] ------- [ iol4 ]
#   (pagent1)          (uut1)          (uut2)         (pagent1)
#             link12           link23           link34
#

global topology
if (__name__ == "__main__"):
    topology = xrut.ng_topology_t( {'link12': ( 'iol1', 'iol2' ),
                                    'link23': ( 'iol2', 'iol3' ),
                                    'link34': ( 'iol3', 'iol4' )},
                                   )

class testbed_config (utng.test_suite_t):
    def __init__ (self):

        # Call parent
        super(testbed_config, self).__init__()

        self.real_routers = False
        print "Running on IOL routers"

        # Interfaces
        self.intf_iol1_link12 = str(routers['iol1'].netifs['link12'])
        self.intf_iol2_link12 = str(routers['iol2'].netifs['link12'])
        self.intf_iol2_link23 = str(routers['iol2'].netifs['link23'])
        self.intf_iol3_link23 = str(routers['iol3'].netifs['link23'])
        self.intf_iol3_link34 = str(routers['iol3'].netifs['link34'])
        self.intf_iol4_link34 = str(routers['iol4'].netifs['link34'])

        # Prefixes
        self.prefix_iol1_link12 = routers['iol1'].netifs['link12'].ipv4_prefix
        self.prefix_iol2_link12 = routers['iol2'].netifs['link12'].ipv4_prefix
        self.prefix_iol2_link23 = routers['iol2'].netifs['link23'].ipv4_prefix
        self.prefix_iol3_link23 = routers['iol3'].netifs['link23'].ipv4_prefix
        self.prefix_iol3_link34 = routers['iol3'].netifs['link34'].ipv4_prefix
        self.prefix_iol4_link34 = routers['iol4'].netifs['link34'].ipv4_prefix

        # IP addresses
        self.ip_iol1_link12 = str(routers['iol1'].netifs['link12'].ipv4_prefix.address)
        self.ip_iol2_link12 = str(routers['iol2'].netifs['link12'].ipv4_prefix.address)
        self.ip_iol2_link23 = str(routers['iol2'].netifs['link23'].ipv4_prefix.address)
        self.ip_iol3_link23 = str(routers['iol3'].netifs['link23'].ipv4_prefix.address)
        self.ip_iol3_link34 = str(routers['iol3'].netifs['link34'].ipv4_prefix.address)
        self.ip_iol4_link34 = str(routers['iol4'].netifs['link34'].ipv4_prefix.address)

        # List of routers
        self.rtr_lst = ['iol1', 'iol2', 'iol3', 'iol4']
     
        self.uut_list = ['iol2', 'iol3']

        # Test timeout
        self.std_wait_time = 10


    def check_metadata_on_routers (rtrs, check):
        """ This function returns the predicate to check if metadata is enabled"""
 
        print  "############################################"
        print  "#      Check Metadata enabled on UUTs      #"
        print  "############################################"
 
        # Check that metadata is not listed under <show ?>
        if not (type(rtrs) == list):
            rtrs = [rtrs] 
 
        preq_metadata = [] 
        for router in rtrs:
            preq_metadata.append(metadata.check_command_opt_pred_t(routers[router],
                                                                   'show',
                                                                   'metadata',
                                                                   check),
                                 self.std_wait_time)
        return preq_metadata

    def enable_metata_on_routers (rtrs):
        """This function enables metadata on UUTs"""

        print  "############################################"
        print  "#      Enable Metadata on UUTs             #"
        print  "############################################"

        if not (type(rtrs) == list):
            rtrs = [rtrs]
        preq = []

        for router in rtrs:
            # Enable metadata
            topology.config_topology({routers[router]:'metadata flow'})
            topology.config_topology({routers[router]:'metadata flow transmit'})

        preq_metadata = check_metadata_on_routers(self.uut_list, True)
        preq = pre.prerequisite_t(preq_metadata, self.std_wait_time)

        self.assert_test_case(preq, "Metadata not enabled")

    def config_class_policy_map (rtrs, class_map, policy_map, app):
        """Config class maps on the UUTs"""

        print  "############################################"
        print  "#   Config class-map/policy-map on UUTs    #"
        print  "############################################"

        if not (type(rtrs) == list):
            rtrs = [rtrs]
        preq = []

        for router in rtrs:
            test_config = {
                routers[router]:"""
                                class-map match-all """+ class_map +"""
                                   match-application """+ app +"""
                                policy-map """+ policy_map +"""
                                   class """+ class_map +"""
                                """,
                          }

            result = topology.config_topology_return_rollback(test_config)
            self.assert_test_case(result != None,
                                  "Class/Policy Map could not be configured")


    def config_service_policy (policy_map):
        """Config Service-policy on the egress interface of UUT1"""
        """Config Service-policy on the ingress interface of UUT2"""

        print  "############################################"
        print  "#   Config service-policy input on UUTs    #"
        print  "############################################"

        test_config = {
                    "iol2":"""
                           interface """+ self.intf_iol2_link23 +"""
                             service-policy input """+ policy_map +"""
                           """,
                    "iol3":"""
                           interface """+ self.intf_iol3_link23 +"""
                             service-policy input """+ policy_map +""" 
                           """,
                      }

        result = topology.config_topology_return_rollback(test_config)
        self.assert_test_case(result != None,
                              "service-policy input could not be configured")



###############################################################################
#
# 4. Define the test suite
#
###############################################################################

class metadata_nbar_regression (testbed_config):
    # Required config for the test suite
    "Metadata NBAR Regression"

    #wait for the Interfaces to be UP
    required_prerequisites = pre.prerequisite_t([
        pre.interface_up_pred_t(routers['iol-pagent1'], routers['iol-pagent1'].netifs['link_p1_i1']),
        pre.interface_up_pred_t(routers['iol1'], routers['iol1'].netifs['link_p1_i1']),
        pre.interface_up_pred_t(routers['iol1'], routers['iol1'].netifs['link1_2']),
        pre.interface_up_pred_t(routers['iol2'], routers['iol2'].netifs['link1_2'])
        ], 30)


    def test_1_metadata_nbar (self): 
        """
        Test Steps:
        1  Enable Metadata and Metadata flow transmit on UUT1 & UUT2
        2  Configure class map match-all C1 with match application 
           citrix and polic-map p1 on UUT1 & UUT2
        3  Configure Service policy input <polic-map> on ingress
           interface of UUT1 & UUT2
        4  Verify no packets before traffic initializes
        5  Initiate Traffic
        6  Verify packets after traffic initializes
        7  Verify metadata flow table entry
        8  Clear traffic
        9  Verify metadata flow and local-flow-id detail
       10  Unconfigure metadata related configs
       11  Repeat steps 1-10 with match application RTSP
       12  Repeat steps 1-10 with match application webex-meeting
       13  Repeat steps 1-10 with match(any) application RTP
       13  Repeat steps 1-10 with match(any) application telepresence-control & SIP
       14  Repeat steps 1-10 with match(all) application SIP & H323
       15  Repeat steps 1-10 with match all applications
                Initiate all traffic kinds one after another
       """


#
# 5. Run the test suite
#

if (__name__ == "__main__"):
    metadata_nbar_regression_suite = metadata_nbar_regression()

    # Returns an error code if there is a fatal exception or if one of the tests fails
    rc = topology.execute_test(metadata_nbar_regression_suite);
    sys.exit(rc)


