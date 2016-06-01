/*
 * This file was generated automatically by xsubpp version 1.9507 from the 
 * contents of Random.xs. Do not edit this file, edit Random.xs instead.
 *
 *	ANY CHANGES MADE HERE WILL BE LOST! 
 *
 */

#line 1 "Random.xs"
#ifdef __cplusplus
extern "C" {
#endif
#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"
#ifdef __cplusplus
}
#endif

#include "randlib.h"
#include "helper.h"

static int
not_here(s)
char *s;
{
    croak("%s not implemented on this architecture", s);
    return -1;
}

static double
constant(name, arg)
char *name;
int arg;
{
    errno = 0;
    switch (*name) {
    }
    errno = EINVAL;
    return 0;

not_there:
    errno = ENOENT;
    return 0;
}


#line 49 "Random.c"
XS(XS_Math__Random_genbet)
{
    dXSARGS;
    if (items != 2)
	Perl_croak(aTHX_ "Usage: Math::Random::genbet(aa, bb)");
    {
	double	aa = (double)SvNV(ST(0));
	double	bb = (double)SvNV(ST(1));
	double	RETVAL;
	dXSTARG;

	RETVAL = genbet(aa, bb);
	XSprePUSH; PUSHn((double)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_genchi)
{
    dXSARGS;
    if (items != 1)
	Perl_croak(aTHX_ "Usage: Math::Random::genchi(df)");
    {
	double	df = (double)SvNV(ST(0));
	double	RETVAL;
	dXSTARG;

	RETVAL = genchi(df);
	XSprePUSH; PUSHn((double)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_genexp)
{
    dXSARGS;
    if (items != 1)
	Perl_croak(aTHX_ "Usage: Math::Random::genexp(av)");
    {
	double	av = (double)SvNV(ST(0));
	double	RETVAL;
	dXSTARG;

	RETVAL = genexp(av);
	XSprePUSH; PUSHn((double)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_genf)
{
    dXSARGS;
    if (items != 2)
	Perl_croak(aTHX_ "Usage: Math::Random::genf(dfn, dfd)");
    {
	double	dfn = (double)SvNV(ST(0));
	double	dfd = (double)SvNV(ST(1));
	double	RETVAL;
	dXSTARG;

	RETVAL = genf(dfn, dfd);
	XSprePUSH; PUSHn((double)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_gengam)
{
    dXSARGS;
    if (items != 2)
	Perl_croak(aTHX_ "Usage: Math::Random::gengam(a, r)");
    {
	double	a = (double)SvNV(ST(0));
	double	r = (double)SvNV(ST(1));
	double	RETVAL;
	dXSTARG;

	RETVAL = gengam(a, r);
	XSprePUSH; PUSHn((double)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_psetmn)
{
    dXSARGS;
    if (items != 1)
	Perl_croak(aTHX_ "Usage: Math::Random::psetmn(p)");
    {
	long	p = (long)SvIV(ST(0));
	int	RETVAL;
	dXSTARG;

	RETVAL = psetmn(p);
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_pgenmn)
{
    dXSARGS;
    if (items != 0)
	Perl_croak(aTHX_ "Usage: Math::Random::pgenmn()");
    {
	int	RETVAL;
	dXSTARG;
#line 80 "Random.xs"
	RETVAL = pgenmn();
#line 159 "Random.c"
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_rspriw)
{
    dXSARGS;
    if (items != 1)
	Perl_croak(aTHX_ "Usage: Math::Random::rspriw(size)");
    {
	long	size = (long)SvIV(ST(0));
	int	RETVAL;
	dXSTARG;

	RETVAL = rspriw(size);
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_rsprfw)
{
    dXSARGS;
    if (items != 1)
	Perl_croak(aTHX_ "Usage: Math::Random::rsprfw(size)");
    {
	long	size = (long)SvIV(ST(0));
	int	RETVAL;
	dXSTARG;

	RETVAL = rsprfw(size);
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_svprfw)
{
    dXSARGS;
    if (items != 2)
	Perl_croak(aTHX_ "Usage: Math::Random::svprfw(index, value)");
    {
	long	index = (long)SvIV(ST(0));
	double	value = (double)SvNV(ST(1));

	svprfw(index, value);
    }
    XSRETURN_EMPTY;
}

XS(XS_Math__Random_pgnmul)
{
    dXSARGS;
    if (items != 2)
	Perl_croak(aTHX_ "Usage: Math::Random::pgnmul(n, ncat)");
    {
	long	n = (long)SvIV(ST(0));
	long	ncat = (long)SvIV(ST(1));

	pgnmul(n, ncat);
    }
    XSRETURN_EMPTY;
}

XS(XS_Math__Random_gvpriw)
{
    dXSARGS;
    if (items != 1)
	Perl_croak(aTHX_ "Usage: Math::Random::gvpriw(index)");
    {
	long	index = (long)SvIV(ST(0));
	long	RETVAL;
	dXSTARG;

	RETVAL = gvpriw(index);
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_gennch)
{
    dXSARGS;
    if (items != 2)
	Perl_croak(aTHX_ "Usage: Math::Random::gennch(df, xnonc)");
    {
	double	df = (double)SvNV(ST(0));
	double	xnonc = (double)SvNV(ST(1));
	double	RETVAL;
	dXSTARG;

	RETVAL = gennch(df, xnonc);
	XSprePUSH; PUSHn((double)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_gennf)
{
    dXSARGS;
    if (items != 3)
	Perl_croak(aTHX_ "Usage: Math::Random::gennf(dfn, dfd, xnonc)");
    {
	double	dfn = (double)SvNV(ST(0));
	double	dfd = (double)SvNV(ST(1));
	double	xnonc = (double)SvNV(ST(2));
	double	RETVAL;
	dXSTARG;

	RETVAL = gennf(dfn, dfd, xnonc);
	XSprePUSH; PUSHn((double)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_gennor)
{
    dXSARGS;
    if (items != 2)
	Perl_croak(aTHX_ "Usage: Math::Random::gennor(av, sd)");
    {
	double	av = (double)SvNV(ST(0));
	double	sd = (double)SvNV(ST(1));
	double	RETVAL;
	dXSTARG;

	RETVAL = gennor(av, sd);
	XSprePUSH; PUSHn((double)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_pgnprm)
{
    dXSARGS;
    if (items != 1)
	Perl_croak(aTHX_ "Usage: Math::Random::pgnprm(n)");
    {
	long	n = (long)SvIV(ST(0));
#line 136 "Random.xs"
	pgnprm(n);
#line 302 "Random.c"
    }
    XSRETURN_EMPTY;
}

XS(XS_Math__Random_genunf)
{
    dXSARGS;
    if (items != 2)
	Perl_croak(aTHX_ "Usage: Math::Random::genunf(low, high)");
    {
	double	low = (double)SvNV(ST(0));
	double	high = (double)SvNV(ST(1));
	double	RETVAL;
	dXSTARG;

	RETVAL = genunf(low, high);
	XSprePUSH; PUSHn((double)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_ignpoi)
{
    dXSARGS;
    if (items != 1)
	Perl_croak(aTHX_ "Usage: Math::Random::ignpoi(mu)");
    {
	double	mu = (double)SvNV(ST(0));
	long	RETVAL;
	dXSTARG;

	RETVAL = ignpoi(mu);
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_ignuin)
{
    dXSARGS;
    if (items != 2)
	Perl_croak(aTHX_ "Usage: Math::Random::ignuin(low, high)");
    {
	long	low = (long)SvIV(ST(0));
	long	high = (long)SvIV(ST(1));
	long	RETVAL;
	dXSTARG;

	RETVAL = ignuin(low, high);
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_ignnbn)
{
    dXSARGS;
    if (items != 2)
	Perl_croak(aTHX_ "Usage: Math::Random::ignnbn(n, p)");
    {
	long	n = (long)SvIV(ST(0));
	double	p = (double)SvNV(ST(1));
	long	RETVAL;
	dXSTARG;

	RETVAL = ignnbn(n, p);
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_ignbin)
{
    dXSARGS;
    if (items != 2)
	Perl_croak(aTHX_ "Usage: Math::Random::ignbin(n, pp)");
    {
	long	n = (long)SvIV(ST(0));
	double	pp = (double)SvNV(ST(1));
	long	RETVAL;
	dXSTARG;

	RETVAL = ignbin(n, pp);
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}

XS(XS_Math__Random_phrtsd)
{
    dXSARGS;
    if (items != 1)
	Perl_croak(aTHX_ "Usage: Math::Random::phrtsd(phrase)");
    SP -= items;
    {
	char *	phrase = (char *)SvPV(ST(0),PL_na);
#line 174 "Random.xs"
	long  newseed1;
	long  newseed2;
#line 402 "Random.c"
#line 177 "Random.xs"
	phrtsd(phrase,&newseed1,&newseed2);
	EXTEND(sp, 2);
	PUSHs(sv_2mortal(newSViv(newseed1)));
	PUSHs(sv_2mortal(newSViv(newseed2)));
#line 408 "Random.c"
	PUTBACK;
	return;
    }
}

XS(XS_Math__Random_getsd)
{
    dXSARGS;
    if (items != 0)
	Perl_croak(aTHX_ "Usage: Math::Random::getsd()");
    SP -= items;
    {
#line 186 "Random.xs"
	long  newseed1;
	long  newseed2;
#line 424 "Random.c"
#line 189 "Random.xs"
	getsd(&newseed1,&newseed2);
	EXTEND(sp, 2);
	PUSHs(sv_2mortal(newSViv(newseed1)));
	PUSHs(sv_2mortal(newSViv(newseed2)));
#line 430 "Random.c"
	PUTBACK;
	return;
    }
}

XS(XS_Math__Random_salfph)
{
    dXSARGS;
    if (items != 1)
	Perl_croak(aTHX_ "Usage: Math::Random::salfph(phrase)");
    {
	char *	phrase = (char *)SvPV(ST(0),PL_na);
#line 200 "Random.xs"
	salfph(phrase);
#line 445 "Random.c"
    }
    XSRETURN_EMPTY;
}

XS(XS_Math__Random_setall)
{
    dXSARGS;
    if (items != 2)
	Perl_croak(aTHX_ "Usage: Math::Random::setall(iseed1, iseed2)");
    {
	long	iseed1 = (long)SvIV(ST(0));
	long	iseed2 = (long)SvIV(ST(1));
#line 210 "Random.xs"
	setall(iseed1,iseed2);
#line 460 "Random.c"
    }
    XSRETURN_EMPTY;
}

XS(XS_Math__Random_gvprfw)
{
    dXSARGS;
    if (items != 1)
	Perl_croak(aTHX_ "Usage: Math::Random::gvprfw(index)");
    {
	long	index = (long)SvIV(ST(0));
	double	RETVAL;
	dXSTARG;

	RETVAL = gvprfw(index);
	XSprePUSH; PUSHn((double)RETVAL);
    }
    XSRETURN(1);
}

#ifdef __cplusplus
extern "C"
#endif
XS(boot_Math__Random)
{
    dXSARGS;
    char* file = __FILE__;

    XS_VERSION_BOOTCHECK ;

        newXS("Math::Random::genbet", XS_Math__Random_genbet, file);
        newXS("Math::Random::genchi", XS_Math__Random_genchi, file);
        newXS("Math::Random::genexp", XS_Math__Random_genexp, file);
        newXS("Math::Random::genf", XS_Math__Random_genf, file);
        newXS("Math::Random::gengam", XS_Math__Random_gengam, file);
        newXS("Math::Random::psetmn", XS_Math__Random_psetmn, file);
        newXSproto("Math::Random::pgenmn", XS_Math__Random_pgenmn, file, "");
        newXS("Math::Random::rspriw", XS_Math__Random_rspriw, file);
        newXS("Math::Random::rsprfw", XS_Math__Random_rsprfw, file);
        newXS("Math::Random::svprfw", XS_Math__Random_svprfw, file);
        newXS("Math::Random::pgnmul", XS_Math__Random_pgnmul, file);
        newXS("Math::Random::gvpriw", XS_Math__Random_gvpriw, file);
        newXS("Math::Random::gennch", XS_Math__Random_gennch, file);
        newXS("Math::Random::gennf", XS_Math__Random_gennf, file);
        newXS("Math::Random::gennor", XS_Math__Random_gennor, file);
        newXSproto("Math::Random::pgnprm", XS_Math__Random_pgnprm, file, "$");
        newXS("Math::Random::genunf", XS_Math__Random_genunf, file);
        newXS("Math::Random::ignpoi", XS_Math__Random_ignpoi, file);
        newXS("Math::Random::ignuin", XS_Math__Random_ignuin, file);
        newXS("Math::Random::ignnbn", XS_Math__Random_ignnbn, file);
        newXS("Math::Random::ignbin", XS_Math__Random_ignbin, file);
        newXSproto("Math::Random::phrtsd", XS_Math__Random_phrtsd, file, "$");
        newXSproto("Math::Random::getsd", XS_Math__Random_getsd, file, "");
        newXSproto("Math::Random::salfph", XS_Math__Random_salfph, file, "$");
        newXSproto("Math::Random::setall", XS_Math__Random_setall, file, "$$");
        newXS("Math::Random::gvprfw", XS_Math__Random_gvprfw, file);
    XSRETURN_YES;
}

