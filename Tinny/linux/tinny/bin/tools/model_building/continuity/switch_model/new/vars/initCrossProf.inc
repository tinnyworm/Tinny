FUNCTION INIT_CROSS_PROF: FLOAT;
VAR
   RET_VAL : NUMERIC;
BEGIN

    RET_VAL := 0;
    IF ((AUTH_RECENT_UPDATE_DATE <> FPP_RECENT_UPDATE_DATE)
        OR (AUTH_RECENT_UPDATE_TIME <> FPP_RECENT_UPDATE_TIME)) THEN
    BEGIN

        PROF.P14              := 0;
        PROF.P15              := 0;
        PROF.P16              := 0;
        PROF.P17              := 0;
        PROF.P18              := 0;
        PROF.P19              := 0;
        PROF.P20              := 0;
        PROF.P21              := 0;
        PROF.P22              := 0;
        PROF.P23              := 0;
        PROF.P24              := 0;
        PROF.P25              := 0;
        PROF.P26              := 0;
        PROF.P27              := 0;
        PROF.P28              := 0;
        PROF.P29              := 0;
        PROF.P30              := 0;
        PROF.P31              := 0;
        PROF.P32              := 0;
        PROF.P33              := 0;
        PROF.P34              := 0;
        PROF.P35              := 0;
        PROF.P36              := 0;
        PROF.P37              := 0;
        PROF.P38              := 0;
        PROF.P39              := 0;
        PROF.P40              := 0;
        PROF.P41              := 0;
        PROF.P42              := 0;
        PROF.P43              := 0;
        PROF.P44              := 0;
        PROF.P45              := 0;
        PROF.P46              := 0;
        PROF.P47              := 0;
        PROF.P48              := 0;
        PROF.P49              := 0;
        PROF.P50              := 0;
        PROF.P51              := 0;
        PROF.P52              := 0;
        PROF.P53              := 0;
        PROF.P54              := 0;
        PROF.P55              := 0;
        PROF.P56              := 0;
        PROF.P57              := 0;
        PROF.P58              := 0;
        PROF.P59              := 0;
        PROF.P60              := 0;
        PROF.P61              := 0;
        PROF.P62              := 0;
        PROF.P63              := 0;
        PROF.P64              := 0;
        PROF.P65              := 0;
        PROF.P66              := 0;
        PROF.P67              := 0;
        PROF.P68              := 0;
        PROF.P69              := 0;

        IF((CURRENT_DATE - CARD_OPNDATE_98S) > 90.0) THEN
            FPP_START_DATE := CURRENT_DATE - 90.0;
        ELSE
            FPP_START_DATE := CARD_OPNDATE_98S;

        FPP_RECENT_UPDATE_DATE := FPP_START_DATE;
        FPP_RECENT_UPDATE_TIME := 0.0;

        RET_VAL := 1;
    END;

    RETURN(RET_VAL);
ENDRULE;

