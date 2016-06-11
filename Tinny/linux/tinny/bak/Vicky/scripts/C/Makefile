CC = gcc

ENGSRC = creatct.c creatrb.c engerr.c execrb.c exptree.c freect.c redcvar.c \
freerb.c lexrule.c prsrule.c rbdebug.c runct.c runfunc.c strstmt.c \
typechk.c datcalc.c evlarg.c evlexpr.c evlstmt.c
ENGOBJ = creatct.o creatrb.o engerr.o execrb.o exptree.o freect.o redcvar.o \
freerb.o lexrule.o prsrule.o rbdebug.o runct.o runfunc.o strstmt.o \
typechk.o datcalc.o evlarg.o evlexpr.o evlstmt.o

PRDSRC = prdcalc.c prdrfnc.c prdmfnc.c falcalc.c falmfnc.c initnet.c freenet.c score.c initprf.c
PRDOBJ = prdcalc.o prdrfnc.o prdmfnc.o falcalc.o falmfnc.o initnet.o freenet.o score.o initprf.o

DPLYSRC = optinet.c
DPLYOBJ = optinet.o

ITMSRC = itmutil.c mymall.c
ITMOBJ = itmutil.o mymall.o

GUTSSRC = config.c except.c ismdata.c mgmodel.c model.c varcalc.c modout.c \
modfunc.c modtab.c rulfunc.c rulpars.c dyntabl.c safopen.c dates.c encrypt.c
GUTSOBJ = config.o except.o ismdata.o mgmodel.o model.o varcalc.o modout.o \
modfunc.o modtab.o rulfunc.o rulpars.o dyntabl.o safopen.o dates.o encrypt.o

PMAXSRC = maxmisc.c rulproc.c
PMAXOBJ = maxmisc.o rulproc.o

DRIVESRC = mfdrive.c research.c flib.c setModelVar.c term.c
DRIVEOBJ = mfdrive.o research.o flib.o setModelVar.o term.o

#CFLAGS = -g -DBATCH -DREDUCE -DMPAGE
#CFLAGS = -O -DBATCH -DDEVELOP -DMBNA
#CFLAGS = -O -DBATCH -DDEVELOP -DREDUCE
#CFLAGS = -O -DBATCH -DITM -DLMT4M -pg
#CFLAGS = -O -DBATCH -DITM -DLMT4M -DDEVELOP
#CFLAGS = -O -DBATCH -DITM -DLMT4M
#CFLAGS = -g -DBATCH -DMPAGE 
#CFLAGS = -O -DBATCH -DMPAGE -DREDUCE -DMODELING
#CFLAGS = -O -DBATCH -DMPAGE -DREDUCE -DMODELING  -DDEBUG -DDEVELOP
CFLAGS = -O -DBATCH -DMPAGE -DREDUCE -DMODELING

drv_falcon30: $(ENGOBJ) $(DPLYOBJ) $(GUTSOBJ) $(PMAXOBJ) $(DRIVEOBJ) $(PRDOBJ) $(ITMOBJ)
#	purify $(CC) $(ENGOBJ) $(DPLYOBJ) $(GUTSOBJ) $(PMAXOBJ) $(DRIVEOBJ) $(PRDOBJ) $(ITMOBJ) -lm -o $@
#	$(CC) -pg $(ENGOBJ) $(DPLYOBJ) $(GUTSOBJ) $(PMAXOBJ) $(DRIVEOBJ) $(PRDOBJ) $(ITMOBJ) -lm -o $@
	$(CC) $(ENGOBJ) $(DPLYOBJ) $(GUTSOBJ) $(PMAXOBJ) $(DRIVEOBJ) $(PRDOBJ) $(ITMOBJ) -lm -o $@

drv_test: $(ENGOBJ) $(DPLYOBJ) $(GUTSOBJ) $(PMAXOBJ) $(DRIVEOBJ) $(PRDOBJ) $(ITMOBJ)
	$(CC) $(ENGOBJ) $(DPLYOBJ) $(GUTSOBJ) $(PMAXOBJ) $(DRIVEOBJ) $(PRDOBJ) $(ITMOBJ) -lm -o drv_test

depend:
	cat Makefile > make.depends
	gcc -DBATCH -MM $(ENGSRC) $(DPLYSRC) $(GUTSSRC) $(PMAXSRC) $(DRIVESRC) $(PRDSRC) $(ITMSRC) >> make.depends

creatct.o: creatct.c mymall.h exptree.h engine.h engdata.h datcalc.h hncrtyp.h \
  prsrule.h parstrc.h toktype.h engerr.h error.h calctab.h creatct.h freect.h \
  typechk.h bltins.h strstmt.h evlarg.h evlexpr.h except.h hnctype.h runct.h \
  evlstmt.h
creatrb.o: creatrb.c mymall.h exptree.h engine.h engdata.h datcalc.h hncrtyp.h \
  prsrule.h parstrc.h toktype.h engerr.h error.h calctab.h creatct.h runfunc.h \
  except.h hnctype.h
engerr.o: engerr.c mymall.h engerr.h error.h calctab.h parstrc.h toktype.h \
  exptree.h engine.h engdata.h datcalc.h hncrtyp.h typechk.h
execrb.o: execrb.c exptree.h engine.h engdata.h datcalc.h hncrtyp.h prsrule.h \
  parstrc.h toktype.h engerr.h error.h calctab.h runfunc.h runct.h except.h \
  hnctype.h
exptree.o: exptree.c mymall.h engerr.h error.h calctab.h parstrc.h toktype.h \
  exptree.h engine.h engdata.h datcalc.h hncrtyp.h typechk.h except.h hnctype.h
freect.o: freect.c exptree.h engine.h engdata.h datcalc.h hncrtyp.h prsrule.h \
  parstrc.h toktype.h engerr.h error.h calctab.h creatct.h freect.h typechk.h \
  bltins.h
redcvar.o: redcvar.c mymall.h model.h engine.h engdata.h datcalc.h ismdata.h \
  dyntabl.h rulfunc.h optinet.h varcalc.h except.h hnctype.h hncrtyp.h config.h \
  typechk.h parstrc.h toktype.h exptree.h rulpars.h hncdef.h lexrule.h engerr.h \
  error.h calctab.h
freerb.o: freerb.c exptree.h engine.h engdata.h datcalc.h hncrtyp.h prsrule.h \
  parstrc.h toktype.h engerr.h error.h calctab.h runfunc.h freect.h typechk.h \
  except.h hnctype.h
lexrule.o: lexrule.c lexrule.h parstrc.h toktype.h exptree.h engine.h engdata.h \
  datcalc.h hncrtyp.h engerr.h error.h calctab.h typechk.h
prsrule.o: prsrule.c mymall.h exptree.h engine.h engdata.h datcalc.h hncrtyp.h \
  prsrule.h parstrc.h toktype.h lexrule.h engerr.h error.h calctab.h typechk.h \
  creatct.h except.h hnctype.h
rbdebug.o: rbdebug.c
runct.o: runct.c mymall.h exptree.h engine.h engdata.h datcalc.h hncrtyp.h \
  engerr.h error.h calctab.h parstrc.h toktype.h bltins.h typechk.h runct.h \
  except.h hnctype.h
runfunc.o: runfunc.c exptree.h engine.h engdata.h datcalc.h hncrtyp.h prsrule.h \
  parstrc.h toktype.h engerr.h error.h calctab.h runfunc.h except.h hnctype.h
strstmt.o: strstmt.c mymall.h exptree.h engine.h engdata.h datcalc.h hncrtyp.h \
  prsrule.h parstrc.h toktype.h lexrule.h engerr.h error.h calctab.h bltins.h \
  typechk.h except.h hnctype.h strstmt.h
typechk.o: typechk.c mymall.h typechk.h engine.h engdata.h datcalc.h parstrc.h \
  toktype.h exptree.h hncrtyp.h engerr.h error.h calctab.h lexrule.h prsrule.h \
  except.h hnctype.h
datcalc.o: datcalc.c datcalc.h
evlarg.o: evlarg.c evlarg.h calctab.h parstrc.h toktype.h exptree.h engine.h \
  engdata.h datcalc.h hncrtyp.h evlstmt.h evlexpr.h bltins.h typechk.h engerr.h \
  error.h except.h hnctype.h runct.h
evlexpr.o: evlexpr.c evlexpr.h calctab.h parstrc.h toktype.h exptree.h engine.h \
  engdata.h datcalc.h hncrtyp.h bltins.h typechk.h engerr.h error.h except.h \
  hnctype.h runct.h
evlstmt.o: evlstmt.c evlstmt.h calctab.h parstrc.h toktype.h exptree.h engine.h \
  engdata.h datcalc.h hncrtyp.h engerr.h error.h runct.h
optinet.o: optinet.c mymall.h optinet.h optierr.h error.h safopen.h
config.o: config.c mymall.h hnctype.h config.h except.h safopen.h
except.o: except.c except.h hnctype.h
ismdata.o: ismdata.c ismdata.h
mgmodel.o: mgmodel.c mymall.h engine.h engdata.h datcalc.h engerr.h error.h \
  calctab.h parstrc.h toktype.h exptree.h hncrtyp.h mgmodel.h model.h ismdata.h \
  dyntabl.h rulfunc.h optinet.h except.h hnctype.h config.h typechk.h modfunc.h \
  varcalc.h
model.o: model.c mymall.h model.h engine.h engdata.h datcalc.h ismdata.h \
  dyntabl.h rulfunc.h optinet.h modtab.h except.h hnctype.h hncrtyp.h config.h \
  error.h maxmisc.h maxstrc.h rulemgt.h mgmodel.h rulpars.h hncdef.h safopen.h \
  varcalc.h freect.h calctab.h parstrc.h toktype.h exptree.h typechk.h
varcalc.o: varcalc.c modfunc.h rulfunc.h engine.h engdata.h datcalc.h model.h \
  ismdata.h dyntabl.h optinet.h except.h hnctype.h hncrtyp.h varcalc.h config.h \
  dates.h mgmodel.h prdcalc.h
modfunc.o: modfunc.c mymall.h modfunc.h rulfunc.h engine.h engdata.h datcalc.h \
  model.h ismdata.h dyntabl.h optinet.h varcalc.h except.h hnctype.h hncrtyp.h \
  dates.h mgmodel.h prdmfnc.h
modtab.o: modtab.c mymall.h modtab.h model.h engine.h engdata.h datcalc.h \
  ismdata.h dyntabl.h rulfunc.h optinet.h modfunc.h rulpars.h hncdef.h except.h \
  hnctype.h hncrtyp.h config.h varcalc.h typechk.h parstrc.h toktype.h exptree.h \
  safopen.h
rulfunc.o: rulfunc.c mymall.h rulfunc.h engine.h engdata.h datcalc.h mgmodel.h \
  model.h ismdata.h dyntabl.h optinet.h hnctype.h hncrtyp.h except.h typechk.h \
  parstrc.h toktype.h exptree.h dates.h prdrfnc.h
rulpars.o: rulpars.c mymall.h rulpars.h engine.h engdata.h datcalc.h hncdef.h \
  except.h hnctype.h config.h hncrtyp.h dyntabl.h rulfunc.h encrypt.h typechk.h \
  parstrc.h toktype.h exptree.h
dyntabl.o: dyntabl.c mymall.h dyntabl.h rulfunc.h engine.h engdata.h datcalc.h \
  rulpars.h hncdef.h except.h hnctype.h hncrtyp.h engerr.h error.h calctab.h \
  parstrc.h toktype.h exptree.h config.h typechk.h
safopen.o: safopen.c safopen.h
dates.o: dates.c dates.h engine.h engdata.h datcalc.h rulfunc.h hncrtyp.h
encrypt.o: encrypt.c encrypt.h
maxmisc.o: maxmisc.c engine.h engdata.h datcalc.h config.h hnctype.h except.h \
  maxmisc.h maxstrc.h rulemgt.h mgmodel.h model.h ismdata.h dyntabl.h rulfunc.h \
  optinet.h rulpars.h hncdef.h calctab.h parstrc.h toktype.h exptree.h hncrtyp.h \
  mymall.h bltins.h varcalc.h modfunc.h dates.h typechk.h
maxfree.o: maxfree.c maxerr.h error.h profmax.h maxstrc.h rulemgt.h engine.h \
  engdata.h datcalc.h mgmodel.h model.h ismdata.h dyntabl.h rulfunc.h optinet.h \
  rulpars.h hncdef.h rulproc.h
maxinit.o: maxinit.c mymall.h maxerr.h error.h profmax.h maxstrc.h rulemgt.h \
  engine.h engdata.h datcalc.h mgmodel.h model.h ismdata.h dyntabl.h rulfunc.h \
  optinet.h rulpars.h hncdef.h rulproc.h typechk.h parstrc.h toktype.h exptree.h \
  hncrtyp.h
maxpint.o: maxpint.c maxerr.h error.h profmax.h maxstrc.h rulemgt.h engine.h \
  engdata.h datcalc.h mgmodel.h model.h ismdata.h dyntabl.h rulfunc.h optinet.h \
  rulpars.h hncdef.h rulproc.h
maxscor.o: maxscor.c maxerr.h error.h profmax.h maxstrc.h rulemgt.h engine.h \
  engdata.h datcalc.h mgmodel.h model.h ismdata.h dyntabl.h rulfunc.h optinet.h \
  rulpars.h hncdef.h rulproc.h
rulproc.o: rulproc.c mymall.h config.h hnctype.h rulproc.h rulemgt.h engine.h \
  engdata.h datcalc.h mgmodel.h model.h ismdata.h dyntabl.h rulfunc.h optinet.h \
  rulpars.h hncdef.h maxerr.h error.h except.h safopen.h
runmod.o: runmod.c profmax.h
prdcalc.o: prdcalc.c prdcalc.h varcalc.h engine.h engdata.h datcalc.h model.h \
  ismdata.h dyntabl.h rulfunc.h optinet.h falcalc.h
prdrfnc.o: prdrfnc.c prdrfnc.h rulfunc.h engine.h engdata.h datcalc.h
prdmfnc.o: prdmfnc.c prdmfnc.h modfunc.h rulfunc.h engine.h engdata.h datcalc.h \
  model.h ismdata.h dyntabl.h optinet.h falmfnc.h
falcalc.o: falcalc.c falcalc.h prdcalc.h varcalc.h engine.h engdata.h datcalc.h \
  model.h ismdata.h dyntabl.h rulfunc.h optinet.h falmfnc.h prdmfnc.h modfunc.h \
  except.h hnctype.h hncrtyp.h config.h dates.h mgmodel.h
falmfnc.o: falmfnc.c rulfunc.h engine.h engdata.h datcalc.h mgmodel.h model.h \
  ismdata.h dyntabl.h optinet.h hnctype.h hncrtyp.h except.h typechk.h parstrc.h \
  toktype.h exptree.h dates.h mymall.h
itmutil.o: itmutil.c itmutil.h
mymall.o: mymall.c mymall.h
