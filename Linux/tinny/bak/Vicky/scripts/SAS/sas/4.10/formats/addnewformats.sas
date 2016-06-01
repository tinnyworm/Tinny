libname macrolib '.';
libname library '/work/gold/projects/bin/CCDS/4.10/formats/yxc';
options mstored sasmstore=macrolib;
options errors=2 nodate ls=100 ps=1000;

/************************* Add new entries into macrolib.formats **************************/
proc format library= library.formats;
   value $Caipsta    ' ' = 'Blank'
                     'Y' = 'Y: Static Authentication supported'
		     'N' = 'N: Static Authentication not supported'
		     ',' = 'Data does not exist'
		     ';' = 'Data intentionally blank'  
		   other = [$CHAR1.];

   value $Caipdyn    ' ' = 'Blank'
                     'S' = 'S: Static Data Authentication'
		     'D' = 'D: Dynamic Data Authentication(DDA)'
		     ',' = 'Data does not exist'
		     ';' = 'Data intentionally blank'  
		   other = [$CHAR1.];
		   
   value $Caipver    ' ' = 'Blank'
                     'Y' = 'Y: Cardholder verification supported'
		     'N' = 'N: Cardholder verification not supported'
		     ',' = 'Data does not exist'
		     ';' = 'Data intentionally blank'  
		   other = [$CHAR1.];
		   
   value $Caiprsk    ' ' = 'Blank'
                     'Y' = 'Y: Perform process'
		     'N' = 'N: Do not perform process'
		     ',' = 'Data does not exist'
		     ';' = 'Data intentionally blank'  
		   other = [$CHAR1.];
		   
   value $Caipiaa    ' ' = 'Blank'
                     'Y' = 'Y: Supported'
		     'N' = 'N: Not supported'
		     ',' = 'Data does not exist'
		     ';' = 'Data intentionally blank'  
		   other = [$CHAR1.];
		   
   value $Cmedia     ' ' = 'Blank'
                     'M' = 'M: Mag-stripe card'
		     'C' = 'C: Chip card'
		     'V' = 'V: Virtual card'
		     ',' = ',: Data does not exist'
		     ';' = ';: Data intentionally blank'  
		   other = [$CHAR1.];
		   
   value $CVV2_P     ' ' = 'Blank'
                     '0' = '0: CVV2/CVC2/CID Not provided'
		     '1' = '1: CVV2/CVC2/CID Present'
		     '2' = '2: CVV2/CVC2/CID Present not legible'
		     '8' = '8: CVV2/CVC2/CID Not requested'
		     '9' = '9: CVV/CVC2/CID 2 Not present'
		     ',' = ',: Data does not exist'
		     ';' = ';: Data intentionally blank'  
		   other = [$CHAR1.];
		    		   
    value $CVV2_R    ' ' = 'Blank'
                     'M' = 'M: CVV2/CVC2/CID match'
		     'N' = 'N: CVV2/CVC2/CID no match'
		     'P' = 'P: CVV2/CVC2/CID not processed'
		     'S' = 'S: CVV2/CVC2/CID not present'
		     'U' = 'U: Not certified or provided'
		     ',' = ',: Data does not exist'
		     ';' = ';: Data intentionally blank'  
		   other = [$CHAR1.];				   
		   
     value $AVS_RE   ' ' = 'Blank'
                     'X' = 'X: AVS exact'
		     'Y' = 'Y: AVS ZIP five add'
		     'A' = 'A: AVS add only'
		     'W' = 'W: VS add only'
		     'Z' = 'Z: AVS ZIP five only'
		     'N' = 'N: AVS no match'
		     'U' = 'U: AVS no data'
		     'R' = 'R: AVS system unavailable'
		     'S' = 'S: AVS not supported'
		     ',' = ',: Data does not exist'
		     ';' = ';: Data intentionally blank'  
		   other = [$CHAR1.];
		   
     value $TRAN_CA  ' ' = 'Blank'
                     'I' = 'I: Internet'
		     'T' = 'T: Telephone order'
		     'M' = 'M: Mail order'
		     'P' = 'P: Card present'
		     ',' = ',: Data does not exist'
		     ';' = ';: Data intentionally blank'  
		   other = [$CHAR1.];
		   
     value $FRD_TRX  ' ' = 'Blank'
                     'F' = 'F: Fraudulent transaction'
		     'N' = 'N: Not a fraudulent transaction'
		     '&' = '&: Unknown'
		     ',' = ',: Data does not exist'
		     ';' = ';: Data intentionally blank'  
		   other = [$CHAR1.];
		   
     value $POS_CON  ' '  = 'Blank'
                     '00' = '00: Normal transaction'
		     '01' = '01: Customer not present'
		     '03' = '03: Merchant suspicious'
		     '05' = '05: Customer present,card not present'
		     '08' = '08: Mail/telephone order'
		     '10' = '10: Customer identity verified'
		     '51' = '51: Request for account num. or address'
		     '55' = '55: Request for telecode'
		     ','  =  ',: Data does not exist'
		     ';'  =  ';: Data intentionally blank'  
		   other  = [$CHAR1.];
		   
     value $ATM_NET  ' ' = 'Blank'
                     'C' = 'C: Cirrus'
		     'S' = 'S: Star'
		     'B' = 'B: Bank's ATM'
		     'A' = 'A: Another bank's ATM'
		     'I' = 'I: Interlink'
		     'P' = 'P: Plus'
		     'E' = 'E: Pulse'
		     'O' = 'O: Other'
		     other = [$CHAR1.];
  
     value $V_ALID   ' ' = 'Blank'
                     'V' = 'V: Valid'
		     'I' = 'I: Invalid'
		     ',' = ',: Data does not exist'
		     ';' = ';: Data intentionally blank'  
		   other = [$CHAR1.];	
		   
     value $PAY_TP   ' ' = 'Blank: No selection/Unknown'
                     'P' = 'P: Pay off in one payment'
		     'N' = 'N: Pay off in next two payments'
		     'B' = 'B: Pay off with next bonus'
		     'S' = 'S: Pay off with next two bonuses'
		     'R' = 'R: Revolving'
		     'I' = 'I: Pay over several payments'
		     'O' = 'O: Other'
		   other = [$CHAR1.];

     value $CON_MD   ' ' = 'Blank: Unknown'
                     'T' = 'T: Telephone'
		     'I' = 'I: Internet'
		     'O' = 'O: Other'
		   other = [$CHAR1.]; 		     
		   
     value $RES_MD   ' ' = 'Blank'
                     'R' = 'R: Requested'
		     'P' = 'P: Pushed'
		   other = [$CHAR1.];

     value $VTM_TP   ' ' = 'Blank'
                     '0' = '0: Unspecified'
		     '1' = '1: Limited amount terminal'
		     '2' = '2: Unattended terminal(ATM)'
		     '3' = '3: Unattended terminal(Other)'
		     '4' = '4: Eclectronic cash register'
		     '5' = '5: Home terminals'
		     '7' = '7: Telephone device'
		     ',' = ',: Data does not exist'
		     ';' = ';: Data intentionally blank'  
		   other = [$CHAR1.];
		   
     value $MTM_TP   ' ' = 'Blank'
                     '0' = '0: Not a CAT transaction'
		     '1' = '1: Authorized level1 CAT'
		     '2' = '2: Authorized level2 CAT'
		     '3' = '3: Authorized level3 CAT'
		     '4' = '4: Authorized level4 CAT'
		     '6' = '6: Authorized level6 CAT'
		     '7' = '7: Authorized level7 CAT'
		     ',' = ',: Data does not exist'
		     ';' = ';: Data intentionally blank'  
		   other = [$CHAR1.];
		   
     value $VTM_ETY  ' ' = 'Blank'
                     '0' = '0: Unknown'
		     '1' = '1: Terminal not used'
		     '2' = '2: Magnetic stripe read capability'
		     '3' = '3: Bar code read capability'
		     '4' = '4: OCR read capability'
		     '5' = '5: Chip read capability'
		     '9' = '9: Does not read card data'
		     ',' = ',: Data does not exist'
		     ';' = ';: Data intentionally blank'  
		   other = [$CHAR1.];
		   
     value $MTM_ETY  ' ' = 'Blank'
                     '0' = '0: Unknown/Unspecified'
		     '1' = '1: No terminal used'
		     '2' = '2: Magnetic stripe reader'
		     '3' = '3: Bar code'
		     '4' = '4: Optical character recognition'
		     '5' = '5: Magnetic stripe reader & EMV'
		     '6' = '6: Key entry only'
		     '7' = '7: Magnetic stripe reader & key entry'
		     '8' = '8: Magnetic stripe reader & key & EMV'
		     '9' = '9: EMV chip reader'
		     ',' = ',: Data does not exist'
		     ';' = ';: Data intentionally blank'  
		   other = [$CHAR1.];		   
		     
run;
		   	   	   
proc catalog catalog=macrolib.formats;
contents;
run;
		   		   		       
/*proc format 
    library = library.newcard fmtlib;
 select  $Caipsta $Caipdyn $Caipver $Caiprsk $Caipiaa $Cmedia $country $issue;
 run;
