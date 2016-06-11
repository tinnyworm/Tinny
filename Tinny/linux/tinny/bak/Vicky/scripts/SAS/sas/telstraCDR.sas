options ps = 50 ls = 75 nocenter;
%macro mainmac;

%let NumFiles = 2;
%let f1 = adssmb_prod2809; 
%let f2 = adssll_prod2809;
 
%let ofname = adss;

%let dirname = /telecom2/TELSTRA/production_data;

%macro readin(fname,fnum);
 
data f&fnum;
infile "&dirname./&fname..txt" lrecl=2048;
input @1 rectype $1. @;
if rectype = "D" then do;
	input %include "&dirname./layouts/cdr.prod";;
	output;
end;
else input;

%mend readin;

%do ind = 1 %to &NumFiles;
%readin(&&f&ind,&ind);
%end;
data cdr;
set 
%do ind = 1 %to &NumFiles;
f&ind
%end;
;
run;





proc sql;
create table sql123 as
select count(*) as TOTAL_QTY,
       count(carrier) as CARRIER_QTY, 
       count(acct_nbr) as ACCT_QTY, 
       count(svc_id) as SVC_ID_QTY,
       count(card_num) as CARD_NUM_QTY,
       count(card_pin) as CARD_PIN_QTY,
       count(tns_code) as TNS_QTY,
       count(otrnkgrp) as ORIG_TRNK_GRP_QTY,
       count(otrnkcrt) as ORIF_TRNK_CRCT_QTY,
       count(ochnlnum) as ORIG_CHANNEL_NUMBER,
       count(ftrnkgrp) as FINAL_TRUNK_GROUP,
       count(ftrnkcrt) as FINAL_TRUNK_CIRCUIT,
       count(fchnlnum) as FINAL_CHANNEL_NUMBER,
       count(msisdn) as  MOBILE_SUBSCRIBER_ISDN,
       count(bill_num) as BILL_NUM,
       count(orig_imsi_min) as ORIG_IMSI_MIN,
       count(term_imsi_min) as TERM_IMSI_MIN,
       count(orig_num) as ORIG_NUM,
       count(dialdnum) as DIALED_NUM,
       count(loc_rt_num) as LOCAL_ROUTING_NUMBER,
       count(mdn) as  MOBILE_DIRECTORY_NUMBER,
       count(termnum) as TERM_NUM,    
       count(rlt_term_num) as RE__TRNK_TERM_QTY,
       count(rlt_bill_num) as RLT_BILL_QTY,
       count(thrd_pty_num) as THIRD_PARTY_NUMBER_QTY,
       count(esn) as ORIG_IMEI_ESN_QTY,
       count(term_esn) as TERM_IMEI_ESN_QTY,
       count(home_site) as  HOME_SITE_QTY,
	count(serv_site) as SERVING_SITE_QTY,
 	count(orig_lac) as  ORIG_LOCATION_AREA_CODE_QTY,
 	count(term_lac) as  TERM_LOCATION_AREA_CODE_QTY,
	count(sid ) as  SERVING_SYSTEM_ID_QTY,
	count(serv_bid) as  SERVING_BILLING_ID_QTY,
	count(fi_iswitch) as  FAIR_ISAAC_INIT_SWITCH_ID_QTY,
	count(iswitch) as INIT_SWITCH_ID_QTY,
	count(icell ) as  INIT_CELL_ID_QTY,
	count(ilat) as  INIT_CELL_LAT_QTY,
	count(ilong) as  INIT_CELL_LONG_QTY,
	count(fswitch) as FINAL_SWITCH_ID_QTY,
	count(fcell) as FINAL_CELL_ID_QTY,
	count(handoff) as  HANDOFF_NUM_QTY,
	count(org_ptcd ) as  ORIG_POINT_CODE_QTY,
	count(trm_ptcd) as  TERM_POINT_CODE_QTY,
	count(tcic) as  Cir_ID _QTY,
	count(origcit ) as  ORIG_CITY_QTY,
	count(origst) as ORIG_STATE_QTY,
	count(origzip ) as  ORIG_ZIP_QTY,
	count(origcnty) as ORIG_CNTRY_QTY,
	count(termcit) as  TERM_CITY_QTY,
	count(termst) as  TERM_STATE_QTY,
	count(termzip) as TERM_ZIP_QTY,
	count(termcnty) as  TERM_CNTRY_QTY,
	count(ttrnkgrp) as  TERM_TRUNK_GROUP_QTY,   
     	count(ttrnkcrt ) as  TERM_TRUNK_CIRCUIT_QTY,
	count(fi_tswitch) as  FAIR_ISAAC_TERM_SWITCH_ID_QTY,
	count(tswitch) as  TERM_SWITCH_ID_QTY,
 	count(tcell) as  TERM_CELL_ID_QTY,
 	count(tlat) as TERM_CELL_LAT_QTY,
	count(tlong) as  TERM_CELL_LONG_QTY,
	count(strtdate) as  CALL_START_DATE_QTY,
 	count(strttime) as CALL_START_TIME_QTY,
	count(call_dur) as  CALL_URATION_QTY,
 	count(hold_dur) as  HOLD_URATION_QTY,
	count(enddate) as  CALL_END_DATE_QTY,
	count(endtime ) as CALL_END_TIME_QTY,
 	count(origgmt) as  ORIG_GMT_OFFSET_QTY,   
    	count(termgmt) as  TERM_GMT_OFFSET_QTY,   
    	count(event_seg) as  EVENT_SEGMENT_QTY,   
     	count(orig_msc_id) as  ORIG_MOBILE_SWITCH_STATION_ID_QTY,
	count(term_msc_id) as TERM_MOBILE_SWITCH_STATION_ID_QTY,
	count(sgsn ) as  SGSN_ADDRESS_QTY,
 	count(ggsn ) as  GGSN_ADDRESS_QTY,
	count(serv_pdp ) as  SERVING_PDP_ADDRESS_QTY,
	count(charge_id ) as  GGSN_PDP_CHARGING_ID_QTY,
 	count(access_pt) as  SSGN_ACCESS_POINT_QTY,
    	count(rec_seq_num ) as  SGSN_GGSN_SEQ_NUMBER _QTY,
 	count(tot_upvol) as  TOTAL_DATA_PDP_UP_VOLUME_QTY,
	count(tot_dnvol) as  TOTAL_DATA_PDP_DOWN_VOLUME_QTY,
 	count(req_qos) as  REQUESTED_QUALITY_OF_SERVICE_QTY,
	count(sel_qos) as SELECTED_QUALITY_OF_SERVICE_QTY,
	count(qosdnbit ) as QUALITY_OF_SERVICE_DOWN_BITRATE_QTY,
	count(qosupbit) as  QUALITY_OF_SERVICE_UP_BITRATE_QTY,
    	count(sms_num) as  NUMBER_SMS_MESSAGES_QTY,
	count(chrgunit) as  CHARGE_UNIT_QTY,
	count(rateplan) as  RATE_PLAN_ID_QTY,
	count(rateplwt) as  RATE_PLAN_WEIGHT_QTY,
	count(rating) as  CALL_RATING_QTY,
    	count(userdata1) as  USERDATA1_QTY,
     	count(userdata2) as USERDATA2_QTY,      
     	count(userdata3) as  USERDATA3_QTY,     
     	count(userdata4) as USERDATA4_QTY
  
 from cdr;
 quit;

ods rtf file="&dirname./rtf/&ofname..rtf";

proc print data=sql123;
title "CDR file: &ofname ";
run;
proc print data=cdr(obs=25);
title "CDR file: &ofname ";
run;
proc freq data=cdr;
tables
     carrier        
     billtype       
     cust_seg1 
     carr_code     
     otrnkcls       
     ftrnkcls       
     org_loc        
     org_calltype   
     inoutind       
     callg_nat      
     calld_nat      
     charg_nat      
     oli_digits     
     calldir        
     rlt_pres       
     wtype          
     wtech          
     ttrnkcls       
     odltsvgs
     tdltsvgs
     juris
     intlflag       
     callport       
     chrg_prty      
     callroam       
     callfwd        
     callwait       
     confcall       
     vmail          
     cam_init       
     pincode        
     vip_flag       
     accttype       
     hotnum         
     warmnum        
     hotcell        
     hotcntry       
     hotimei        
     termstat       
     carrier_sel    
     tekno_disp     
     raterprd       
     valueadd       
     pdp_type       
     qosmean        
     qospeak        
     qosprec        
     chrgtype       
     async          
     ;
 run;
 
 
ods rtf close;

run;

%mend mainmac;


%mainmac;
