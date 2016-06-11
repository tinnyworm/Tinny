/*************************************************************************/
/*****                                                               *****/
/***** FDR   PROFILE READ                                            *****/
/*****                                                               *****/
/*****                                                               *****/
/*****                                                               *****/
/***** 980806                                                        *****/
/***** Emily Bung                                                    *****/
/*************************************************************************/


OPTIONS /* LINESIZE=100 */
         LINESIZE=80 
        /* PS=60 */
        PS=MAX
        /* DATE */
        PAGENO=1
        LABEL
        OBS=max
        COMPRESS=yes
        NOCENTER
        /* below added by mxs */
        NODATE
        NONUMBER
        FORMDLIM="-";

title ;

DATA pro1;
     INFILE STDIN     recfm=f lrecl=1227 MISSOVER FIRSTOBS=1;
     INPUT

      @1        account $19.      	   /*     Account Number  */
      @20	PROF001	FLOAT4.      	   /*     PROF_INIT       */
      @24	PROF002	IB4.      	   /*     PROF_INIT       */
      @28	PROF003	FLOAT4.      	   /*     PROF_INIT       */
      @32	PROF004	IB4.      	   /*     PROF_INIT       */
      @36	PROF005	FLOAT4.      	   /*     PROF_INIT       */
      @40	PROF006	FLOAT4.      	   /*     PROF_INIT       */
      @44	PROF007	FLOAT4.      	   /*     PROF_INIT       */
      @48	PROF008	FLOAT4.      	   /*     PROF_INIT       */
      @52	PROF009	FLOAT4.      	   /*     PROF_INIT       */
      @56	PROF010	FLOAT4.      	   /*     PROF_INIT       */
      @60	PROF011	FLOAT4.      	   /*     PROF_INIT       */
      @64	PROF012	FLOAT4.      	   /*     PROF_INIT       */
      @68	PROF013	FLOAT4.      	   /*     PROF_INIT       */
      @72	PROF014	FLOAT4.      	   /*     PROF_INIT       */
      @76	PROF015	FLOAT4.      	   /*     PROF_INIT       */
      @80	PROF016	FLOAT4.      	   /*     PROF_INIT       */
      @84	PROF017	FLOAT4.      	   /*     PROF_INIT       */
      @88	PROF018	FLOAT4.      	   /*     PROF_INIT       */
      @92	PROF019	FLOAT4.      	   /*     PROF_INIT       */
      @96	PROF020	FLOAT4.      	   /*     PROF_INIT       */
      @100	PROF021	FLOAT4.      	   /*     PROF_INIT       */
      @104	PROF022	FLOAT4.      	   /*     PROF_INIT       */
      @108	PROF023	FLOAT4.      	   /*     PROF_INIT       */
      @112	PROF024	FLOAT4.      	   /*     PROF_INIT       */
      @116	PROF025	FLOAT4.      	   /*     PROF_INIT       */
      @120	PROF026	FLOAT4.      	   /*     PROF_INIT       */
      @124	PROF027	FLOAT4.      	   /*     PROF_INIT       */
      @128	PROF028	FLOAT4.      	   /*     PROF_INIT       */
      @132	PROF029	FLOAT4.      	   /*     PROF_INIT       */
      @136	PROF030	FLOAT4.      	   /*     PROF_INIT       */
      @140	PROF031	FLOAT4.      	   /*     PROF_INIT       */
      @144	PROF032	FLOAT4.      	   /*     PROF_INIT       */
      @148	PROF033	FLOAT4.      	   /*     PROF_INIT       */
      @152	PROF034	FLOAT4.      	   /*     PROF_INIT       */
      @156	PROF035	FLOAT4.      	   /*     PROF_INIT       */
      @160	PROF036	FLOAT4.      	   /*     PROF_INIT       */
      @164	PROF037	FLOAT4.      	   /*     PROF_INIT       */
      @168	PROF038	FLOAT4.      	   /*     PROF_INIT       */
      @172	PROF039	FLOAT4.      	   /*     PROF_INIT       */
      @176	PROF040	FLOAT4.      	   /*     PROF_INIT       */
      @180	PROF041	FLOAT4.      	   /*     PROF_INIT       */
      @184	PROF042	FLOAT4.      	   /*     PROF_INIT       */
      @188	PROF043	FLOAT4.      	   /*     PROF_INIT       */
      @192	PROF044	FLOAT4.      	   /*     PROF_INIT       */
      @196	PROF045	FLOAT4.      	   /*     PROF_INIT       */
      @200	PROF046	FLOAT4.      	   /*     PROF_INIT       */
      @204	PROF047	FLOAT4.      	   /*     PROF_INIT       */
      @208	PROF048	FLOAT4.      	   /*     PROF_INIT       */
      @212	PROF049	FLOAT4.      	   /*     PROF_INIT       */
      @216	PROF050	FLOAT4.      	   /*     PROF_INIT       */
      @220	PROF051	FLOAT4.      	   /*     PROF_INIT       */
      @224	PROF052	FLOAT4.      	   /*     PROF_INIT       */
      @228	PROF053	FLOAT4.      	   /*     PROF_INIT       */
      @232	PROF054	FLOAT4.      	   /*     PROF_INIT       */
      @236	PROF055	FLOAT4.      	   /*     PROF_INIT       */
      @240	PROF056	FLOAT4.      	   /*     PROF_INIT       */
      @244	PROF057	FLOAT4.      	   /*     PROF_INIT       */
      @248	PROF058	FLOAT4.      	   /*     PROF_INIT       */
      @252	PROF059	FLOAT4.      	   /*     PROF_INIT       */
      @256	PROF060	FLOAT4.      	   /*     PROF_INIT       */
      @260	PROF061	FLOAT4.      	   /*     PROF_INIT       */
      @264	PROF062	FLOAT4.      	   /*     PROF_INIT       */
      @268	PROF063	FLOAT4.      	   /*     PROF_INIT       */
      @272	PROF064	FLOAT4.      	   /*     PROF_INIT       */
      @276	PROF065	FLOAT4.      	   /*     PROF_INIT       */
      @280	PROF066	FLOAT4.      	   /*     PROF_INIT       */
      @284	PROF067	FLOAT4.      	   /*     PROF_INIT       */
      @288	PROF068	FLOAT4.      	   /*     PROF_INIT       */
      @292	PROF069	FLOAT4.      	   /*     PROF_INIT       */
      @296	PROF070	FLOAT4.      	   /*     PROF_INIT       */
      @300	PROF071	FLOAT4.      	   /*     PROF_INIT       */
      @304	PROF072	FLOAT4.      	   /*     PROF_INIT       */
      @308	PROF073	FLOAT4.      	   /*     PROF_INIT       */
      @312	PROF074	FLOAT4.      	   /*     PROF_INIT       */
      @316	PROF075	FLOAT4.      	   /*     PROF_INIT       */
      @320	PROF076	FLOAT4.      	   /*     PROF_INIT       */
      @324	PROF077	FLOAT4.      	   /*     PROF_INIT       */
      @328	PROF078	FLOAT4.      	   /*     PROF_INIT       */
      @332	PROF079	FLOAT4.      	   /*     PROF_INIT       */
      @336	PROF080	FLOAT4.      	   /*     PROF_INIT       */
      @340	PROF081	FLOAT4.      	   /*     PROF_INIT       */
      @344	PROF082	FLOAT4.      	   /*     PROF_INIT       */
      @348	PROF083	FLOAT4.      	   /*     PROF_INIT       */
      @352	PROF084	FLOAT4.      	   /*     PROF_INIT       */
      @356	PROF085	FLOAT4.      	   /*     PROF_INIT       */
      @360	PROF086	FLOAT4.      	   /*     PROF_INIT       */
      @364	PROF087	FLOAT4.      	   /*     PROF_INIT       */
      @368	PROF088	FLOAT4.      	   /*     PROF_INIT       */
      @372	PROF089	FLOAT4.      	   /*     PROF_INIT       */
      @376	PROF090	FLOAT4.      	   /*     PROF_INIT       */
      @380	PROF091	FLOAT4.      	   /*     PROF_INIT       */
      @384	PROF092	FLOAT4.      	   /*     PROF_INIT       */
      @388	PROF093	FLOAT4.      	   /*     PROF_INIT       */
      @392	PROF094	FLOAT4.      	   /*     PROF_INIT       */
      @396	PROF095	FLOAT4.      	   /*     PROF_INIT       */
      @400	PROF096	FLOAT4.      	   /*     PROF_INIT       */
      @404	PROF097	FLOAT4.      	   /*     PROF_INIT       */
      @408	PROF098	FLOAT4.      	   /*     PROF_INIT       */
      @412	PROF099	FLOAT4.      	   /*     PROF_INIT       */
      @416	PROF100	FLOAT4.      	   /*     PROF_INIT       */
      @420	PROF101	FLOAT4.      	   /*     PROF_INIT       */
      @424	PROF102	FLOAT4.      	   /*     PROF_INIT       */
      @428	PROF103	FLOAT4.      	   /*     PROF_INIT       */
      @432	PROF104	FLOAT4.      	   /*     PROF_INIT       */
      @436	PROF105	FLOAT4.      	   /*     PROF_INIT       */
      @440	PROF106	FLOAT4.      	   /*     PROF_INIT       */
      @444	PROF107	FLOAT4.      	   /*     PROF_INIT       */
      @448	PROF108	FLOAT4.      	   /*     PROF_INIT       */
      @452	PROF109	FLOAT4.      	   /*     PROF_INIT       */
      @456	PROF110	FLOAT4.      	   /*     PROF_INIT       */
      @460	PROF111	FLOAT4.      	   /*     PROF_INIT       */
      @464	PROF112	FLOAT4.      	   /*     PROF_INIT       */
      @468	PROF113	FLOAT4.      	   /*     PROF_INIT       */
      @472	PROF114	FLOAT4.      	   /*     PROF_INIT       */
      @476	PROF115	FLOAT4.      	   /*     PROF_INIT       */
      @480	PROF116	FLOAT4.      	   /*     PROF_INIT       */
      @484	PROF117	FLOAT4.      	   /*     PROF_INIT       */
      @488	PROF118	FLOAT4.      	   /*     PROF_INIT       */
      @492	PROF119	FLOAT4.      	   /*     PROF_INIT       */
      @496	PROF120	FLOAT4.      	   /*     PROF_INIT       */
      @500	PROF121	FLOAT4.      	   /*     PROF_INIT       */
      @504	PROF122	FLOAT4.      	   /*     PROF_INIT       */
      @508	PROF123	FLOAT4.      	   /*     PROF_INIT       */
      @512	PROF124	FLOAT4.      	   /*     PROF_INIT       */
      @516	PROF125	FLOAT4.      	   /*     PROF_INIT       */
      @520	PROF126	FLOAT4.      	   /*     PROF_INIT       */
      @524	PROF127	FLOAT4.      	   /*     PROF_INIT       */
      @528	PROF128	FLOAT4.      	   /*     PROF_INIT       */
      @532	PROF129	FLOAT4.      	   /*     PROF_INIT       */
      @536	PROF130	FLOAT4.      	   /*     PROF_INIT       */
      @540	PROF131	FLOAT4.      	   /*     PROF_INIT       */
      @544	PROF132	FLOAT4.      	   /*     PROF_INIT       */
      @548	PROF133	FLOAT4.      	   /*     PROF_INIT       */
      @552	PROF134	FLOAT4.      	   /*     PROF_INIT       */
      @556	PROF135	FLOAT4.      	   /*     PROF_INIT       */
      @560	PROF136	FLOAT4.      	   /*     PROF_INIT       */
      @564	PROF137	FLOAT4.      	   /*     PROF_INIT       */
      @568	PROF138	FLOAT4.      	   /*     PROF_INIT       */
      @572	PROF139	FLOAT4.      	   /*     PROF_INIT       */
      @576	PROF140	FLOAT4.      	   /*     PROF_INIT       */
      @580	PROF141	FLOAT4.      	   /*     PROF_INIT       */
      @584	PROF142	FLOAT4.      	   /*     PROF_INIT       */
      @588	PROF143	FLOAT4.      	   /*     PROF_INIT       */
      @592	PROF144	FLOAT4.      	   /*     PROF_INIT       */
      @596	PROF145	FLOAT4.      	   /*     PROF_INIT       */
      @600	PROF146	FLOAT4.      	   /*     PROF_INIT       */
      @604	PROF147	FLOAT4.      	   /*     PROF_INIT       */
      @608	PROF148	FLOAT4.      	   /*     PROF_INIT       */
      @612	PROF149	FLOAT4.      	   /*     PROF_INIT       */
      @616	PROF150	FLOAT4.      	   /*     PROF_INIT       */
      @620	PROF151	FLOAT4.      	   /*     PROF_INIT       */
      @624	PROF152	FLOAT4.      	   /*     PROF_INIT       */
      @628	PROF153	FLOAT4.      	   /*     PROF_INIT       */
      @632	PROF154	FLOAT4.      	   /*     PROF_INIT       */
      @636	PROF155	FLOAT4.      	   /*     PROF_INIT       */
      @640	PROF156	FLOAT4.      	   /*     PROF_INIT       */
      @644	PROF157	FLOAT4.      	   /*     PROF_INIT       */
      @648	PROF158	FLOAT4.      	   /*     PROF_INIT       */
      @652	PROF159	FLOAT4.      	   /*     PROF_INIT       */
      @656	PROF160	FLOAT4.      	   /*     PROF_INIT       */
      @660	PROF161	FLOAT4.      	   /*     PROF_INIT       */
      @664	PROF162	FLOAT4.      	   /*     PROF_INIT       */
      @668	PROF163	FLOAT4.      	   /*     PROF_INIT       */
      @672	PROF164	FLOAT4.      	   /*     PROF_INIT       */
      @676	PROF165	FLOAT4.      	   /*     PROF_INIT       */
      @680	PROF166	FLOAT4.      	   /*     PROF_INIT       */
      @684	PROF167	FLOAT4.      	   /*     PROF_INIT       */
      @688	PROF168	FLOAT4.      	   /*     PROF_INIT       */
      @692	PROF169	FLOAT4.      	   /*     PROF_INIT       */
      @696	PROF170	FLOAT4.      	   /*     PROF_INIT       */
      @700	PROF171	FLOAT4.      	   /*     PROF_INIT       */
      @704	PROF172	$1.      	   /*     PROF_INIT       */
      @708	PROF173	FLOAT4.      	   /*     PROF_INIT       */
      @712	PROF174	FLOAT4.      	   /*     PROF_INIT       */
      @716	PROF175	FLOAT4.      	   /*     PROF_INIT       */
      @720	PROF176	FLOAT4.      	   /*     PROF_INIT       */
      @724	PROF177	FLOAT4.      	   /*     PROF_INIT       */
      @728	PROF178	FLOAT4.      	   /*     PROF_INIT       */
      @732	PROF179	FLOAT4.      	   /*     PROF_INIT       */
      @736	PROF180	FLOAT4.      	   /*     PROF_INIT       */
      @740	PROF181	FLOAT4.      	   /*     PROF_INIT       */
      @744	PROF182	FLOAT4.      	   /*     PROF_INIT       */
      @748	PROF183	FLOAT4.      	   /*     PROF_INIT       */
      @752	PROF184	FLOAT4.      	   /*     PROF_INIT       */
      @756	PROF185	FLOAT4.      	   /*     PROF_INIT       */
      @760	PROF186	FLOAT4.      	   /*     PROF_INIT       */
      @764	PROF187	FLOAT4.      	   /*     PROF_INIT       */
      @768	PROF188	FLOAT4.      	   /*     PROF_INIT       */
      @772	PROF189	FLOAT4.      	   /*     PROF_INIT       */
      @776	PROF190	FLOAT4.      	   /*     PROF_INIT       */
      @780	PROF191	FLOAT4.      	   /*     PROF_INIT       */
      @784	PROF192	FLOAT4.      	   /*     PROF_INIT       */
      @788	PROF193	FLOAT4.      	   /*     PROF_INIT       */
      @792	PROF194	FLOAT4.      	   /*     PROF_INIT       */
      @796	PROF195	FLOAT4.      	   /*     PROF_INIT       */
      @800	PROF196	FLOAT4.      	   /*     PROF_INIT       */
      @804	PROF197	FLOAT4.      	   /*     PROF_INIT       */
      @808	PROF198	FLOAT4.      	   /*     PROF_INIT       */
      @812	PROF199	FLOAT4.      	   /*     PROF_INIT       */
      @816	PROF200	FLOAT4.      	   /*     PROF_INIT       */
      @820	PROF201	FLOAT4.      	   /*     PROF_INIT       */
      @824	PROF202	FLOAT4.      	   /*     PROF_INIT       */
      @828	PROF203	FLOAT4.      	   /*     PROF_INIT       */
      @832	PROF204	FLOAT4.      	   /*     PROF_INIT       */
      @836	PROF205	FLOAT4.      	   /*     PROF_INIT       */
      @840	PROF206	FLOAT4.      	   /*     PROF_INIT       */
      @844	PROF207	FLOAT4.      	   /*     PROF_INIT       */
      @848	PROF208	FLOAT4.      	   /*     PROF_INIT       */
      @852	PROF209	FLOAT4.      	   /*     PROF_INIT       */
      @856	PROF210	FLOAT4.      	   /*     PROF_INIT       */
      @860	PROF211	FLOAT4.      	   /*     PROF_INIT       */
      @864	PROF212	FLOAT4.      	   /*     PROF_INIT       */
      @868	PROF213	FLOAT4.      	   /*     PROF_INIT       */
      @872	PROF214	FLOAT4.      	   /*     PROF_INIT       */
      @876	PROF215	FLOAT4.      	   /*     PROF_INIT       */
      @880	PROF216	FLOAT4.      	   /*     PROF_INIT       */
      @884	PROF217	FLOAT4.      	   /*     PROF_INIT       */
      @888	PROF218	FLOAT4.      	   /*     PROF_INIT       */
      @892	PROF219	FLOAT4.      	   /*     PROF_INIT       */
      @896	PROF220	FLOAT4.      	   /*     PROF_INIT       */
      @900	PROF221	FLOAT4.      	   /*     PROF_INIT       */
      @904	PROF222	FLOAT4.      	   /*     PROF_INIT       */
      @908	PROF223	FLOAT4.      	   /*     PROF_INIT       */
      @912	PROF224	FLOAT4.      	   /*     PROF_INIT       */
      @912	PROF225	FLOAT4.      	   /*     PROF_INIT       */
      @920	PROF226	FLOAT4.      	   /*     PROF_INIT       */
      @924	PROF227	FLOAT4.      	   /*     PROF_INIT       */
      @928	PROF228	FLOAT4.      	   /*     PROF_INIT       */
      @932	PROF229	FLOAT4.      	   /*     PROF_INIT       */
      @936	PROF230	FLOAT4.      	   /*     PROF_INIT       */
      @940	PROF231	FLOAT4.      	   /*     PROF_INIT       */
      @944	PROF232	FLOAT4.      	   /*     PROF_INIT       */
      @948	PROF233	FLOAT4.      	   /*     PROF_INIT       */
      @952	PROF234	FLOAT4.      	   /*     PROF_INIT       */
      @956	PROF235	FLOAT4.      	   /*     PROF_INIT       */
      @960	PROF236	FLOAT4.      	   /*     PROF_INIT       */
      @964	PROF237	FLOAT4.      	   /*     PROF_INIT       */
      @968	PROF238	FLOAT4.      	   /*     PROF_INIT       */
      @972	PROF239	FLOAT4.      	   /*     PROF_INIT       */
      @976	PROF240	FLOAT4.      	   /*     PROF_INIT       */
      @980	PROF241	FLOAT4.      	   /*     PROF_INIT       */
      @984	PROF242	FLOAT4.      	   /*     PROF_INIT       */
      @988	PROF243	FLOAT4.      	   /*     PROF_INIT       */
      @992	PROF244	FLOAT4.      	   /*     PROF_INIT       */
      @996	PROF245	FLOAT4.      	   /*     PROF_INIT       */
      @1000	PROF246	FLOAT4.      	   /*     PROF_INIT       */
      @1004	PROF247	FLOAT4.      	   /*     PROF_INIT       */
      @1008	PROF248	FLOAT4.      	   /*     PROF_INIT       */
      @1012	PROF249	FLOAT4.      	   /*     PROF_INIT       */
      @1016	PROF250	FLOAT4.      	   /*     PROF_INIT       */
      @1020	PROF251	FLOAT4.      	   /*     PROF_INIT       */
      @1024	PROF252	FLOAT4.      	   /*     PROF_INIT       */
      @1028	PROF253	FLOAT4.      	   /*     PROF_INIT       */
      @1032	PROF254	FLOAT4.      	   /*     PROF_INIT       */
      @1036	PROF255	FLOAT4.      	   /*     PROF_INIT       */
      @1040	PROF256	FLOAT4.      	   /*     PROF_INIT       */
      @1044	PROF257	FLOAT4.      	   /*     PROF_INIT       */
      @1048	PROF258	FLOAT4.      	   /*     PROF_INIT       */
      @1052	PROF259	FLOAT4.      	   /*     PROF_INIT       */
      @1056	PROF260	FLOAT4.      	   /*     PROF_INIT       */
      @1060	PROF261	FLOAT4.      	   /*     PROF_INIT       */
      @1064	PROF262	FLOAT4.      	   /*     PROF_INIT       */
      @1068	PROF263	FLOAT4.      	   /*     PROF_INIT       */
      @1072	PROF264	FLOAT4.      	   /*     PROF_INIT       */
      @1076	PROF265	FLOAT4.      	   /*     PROF_INIT       */
      @1080	PROF266	FLOAT4.      	   /*     PROF_INIT       */
      @1084	PROF267	FLOAT4.      	   /*     PROF_INIT       */
      @1088	PROF268	FLOAT4.      	   /*     PROF_INIT       */
      @1092	PROF269	FLOAT4.      	   /*     PROF_INIT       */
      @1096	PROF270	FLOAT4.      	   /*     PROF_INIT       */
      @1100	PROF271	FLOAT4.      	   /*     PROF_INIT       */
      @1104	PROF272	FLOAT4.      	   /*     PROF_INIT       */
      @1108	PROF273	FLOAT4.      	   /*     PROF_INIT       */
      @1112	PROF274	FLOAT4.      	   /*     PROF_INIT       */
      @1116	PROF275	FLOAT4.      	   /*     PROF_INIT       */
      @1120	PROF276	FLOAT4.      	   /*     PROF_INIT       */
      @1124	PROF277	FLOAT4.      	   /*     PROF_INIT       */
      @1128	PROF278	FLOAT4.      	   /*     PROF_INIT       */
      @1132	PROF279	FLOAT4.      	   /*     PROF_INIT       */
      @1136	PROF280	FLOAT4.      	   /*     PROF_INIT       */
      @1140	PROF281	FLOAT4.      	   /*     PROF_INIT       */
      @1144	PROF282	FLOAT4.      	   /*     PROF_INIT       */
      @1148	PROF283	FLOAT4.      	   /*     PROF_INIT       */
      @1152	PROF284	FLOAT4.      	   /*     PROF_INIT       */
      @1156	PROF285	FLOAT4.      	   /*     PROF_INIT       */
      @1160	PROF286	FLOAT4.      	   /*     PROF_INIT       */
      @1164	PROF287	FLOAT4.      	   /*     PROF_INIT       */
      @1168	PROF288	FLOAT4.      	   /*     PROF_INIT       */
      @1172	PROF289	FLOAT4.      	   /*     PROF_INIT       */
      @1176	PROF290	FLOAT4.      	   /*     PROF_INIT       */
      @1180	PROF291	FLOAT4.      	   /*     PROF_INIT       */
      @1184	PROF292	FLOAT4.      	   /*     PROF_INIT       */
      @1188	PROF293	FLOAT4.      	   /*     PROF_INIT       */
      @1192	PROF294	FLOAT4.      	   /*     PROF_INIT       */
      @1196	PROF295	FLOAT4.      	   /*     PROF_INIT       */
      @1200	PROF296	FLOAT4.      	   /*     PROF_INIT       */
      @1204	PROF297	FLOAT4.      	   /*     PROF_INIT       */
      @1208	PROF298	FLOAT4.      	   /*     PROF_INIT       */
      @1212	PROF299	FLOAT4.      	   /*     PROF_INIT       */
      @1216	PROF300	FLOAT4.      	   /*     PROF_INIT       */
      @1220	PROF301	FLOAT4.      	   /*     PROF_INIT       */
      @1224	PROF302	FLOAT4.      	   /*     PROF_INIT       */

      ;

      hour = int(PROF010/3600.);
      hunamt = ceil(PROF007/100.);
      huncrlmt = ceil(PROF008/100.);
/*    changed int() to ceil() to differentiate 0's.
      hunamt = int(PROF007/100.);
      huncrlmt = int(PROF008/100.);
*/
      if (PROF003 = 0) then last_s_10=0.0;
/*      last_s_10 = round(PROF003,10); */
      last_s_10 = round(PROF003+4,10); 

      bin = substr(account,1,6);

/*
      numeven=put(PROF221,4.);
      numstrt=put(PROF222,4.);
      numlast=put(PROF009,4.);

      accteven = account || " " || numeven;
      acctstrt = account || " " || numstrt;
      acctlast = account || " " || numlast;
*/
      model_ver = mod(PROF014, 256);
      model_id  = round(PROF014 / 256);
      if (model_ver = 7);
run;
 
proc means data=pro1 n mean std min max range;
run;

proc freq data=pro1;
     tables bin / missing; title2 'BINs';
/* tables account / missing; title 'Account Numbers'; */
     tables last_s_10 / missing; title2 'Last Score Result by 10 bins';
     tables PROF002 / missing; title2 'Recent error'; 
/* tables PROF003 / missing; title 'Last Score Result'; */ 
/* tables PROF004 / missing; title 'Recent Merchant Postal'; */
     tables PROF012 / missing; title2 'Recent Merchant Country';
     tables model_ver / missing; 
     tables model_ver*model_id  / missing;title2 'Recent Model ID + version';
     format PROF002 HEX8. model_id HEX6.;
run;
 
proc freq data=pro1 order=FREQ;
     tables PROF005 / missing; title 'Recent SIC';
run;

proc freq data=pro1;
     tables hunamt / missing; title2 'Last Amount in Hundreds';
     tables huncrlmt / missing; title2 'Credit Limit in Hundreds';
     tables PROF009 / missing; title2 'Recent Auth Update Date';
     tables hour / missing; title2 'Recent Auth Update Hour';
     tables PROF011 / missing; title2 'Auth Number Events Seen';
     tables PROF171 / missing; title2 'Prof_Matr_Date';
     tables PROF172 / missing; title2 'Recent Auth Result';
     tables PROF221 / missing; title2 'Auth Number Events Seen 98S';
     tables PROF222 / missing; title2 'Start Date 98S';
run;
 
/*
proc freq data=pro1;
     tables accteven / missing; title 'Account Number + Number Events 98S';
     tables acctstrt / missing; title 'Account Number + Start Date 98S';
     tables acctlast / missing; title 'Account Number + Last Auth Date';
run;
*/
