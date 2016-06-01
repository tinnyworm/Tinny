#! /bin/csh
    setenv PYTHONPATH /work/gold/projects/lib/
    setenv LD_LIBRARY_PATH "/usr/local/lib:usr/openwin/lib:/usr/lib:/usr/lib/x11"
    
    python /work/applshar/jcl/_requests/mod10check.py | sed "s:|: :"
