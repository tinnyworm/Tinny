FUNCTION INIT_CROSS_PROF: FLOAT;
VAR
   RET_VAL : NUMERIC;
BEGIN

    RET_VAL := 0;
    IF ((AUTH_RECENT_UPDATE_DATE <> FPP_RECENT_UPDATE_DATE)
        OR (AUTH_RECENT_UPDATE_TIME <> FPP_RECENT_UPDATE_TIME)) THEN
    BEGIN

        PROF.P144              := 0;
        PROF.P145              := 0;
        PROF.P146              := 0;
        PROF.P147              := 0;
        PROF.P148              := 0;
        PROF.P149              := 0;
        PROF.P150              := 0;
        PROF.P151              := 0;
        PROF.P152              := 0;
        PROF.P153              := 0;
        PROF.P154              := 0;
        PROF.P155              := 0;
        PROF.P156              := 0;
        PROF.P157              := 0;
        PROF.P158              := 0;
        PROF.P159              := 0;
        PROF.P160              := 0;
        PROF.P161              := 0;
        PROF.P162              := 0;
        PROF.P163              := 0;
        PROF.P164              := 0;
        PROF.P165              := 0;
        PROF.P166              := 0;
        PROF.P167              := 0;
        PROF.P168              := 0;
        PROF.P169              := 0;
        PROF.P170              := 0;
        PROF.P171              := 0;
        PROF.P172              := 0;
        PROF.P173              := 0;
        PROF.P174              := 0;
        PROF.P175              := 0;
        PROF.P176              := 0;
        PROF.P177              := 0;
        PROF.P178              := 0;
        PROF.P179              := 0;
        PROF.P180              := 0;
        PROF.P181              := 0;
        PROF.P182              := 0;
        PROF.P183              := 0;
        PROF.P184              := 0;
        PROF.P185              := 0;
        PROF.P186              := 0;
        PROF.P187              := 0;
        PROF.P188              := 0;
        PROF.P189              := 0;
        PROF.P190              := 0;
        PROF.P191              := 0;
        PROF.P192              := 0;
        PROF.P193              := 0;
        PROF.P194              := 0;
        PROF.P195              := 0;
        PROF.P196              := 0;
        PROF.P197              := 0;
        PROF.P198              := 0;
        PROF.P199              := 0;
        PROF.P200              := 0;
        PROF.P201              := 0;
        PROF.P202              := 0;
        PROF.P203              := 0;
        PROF.P204              := 0;
        PROF.P205              := 0;
        PROF.P206              := 0;
        PROF.P207              := 0;
        PROF.P208              := 0;
        PROF.P209              := 0;
        PROF.P210              := 0;
        PROF.P211              := 0;
        PROF.P212              := 0;
        PROF.P213              := 0;
        PROF.P214              := 0;
        PROF.P215              := 0;
        PROF.P216              := 0;
        PROF.P217              := 0;
        PROF.P218              := 0;
        PROF.P219              := 0;
        PROF.P220              := 0;
        PROF.P221              := 0;
        PROF.P222              := 0;
        PROF.P223              := 0;
        PROF.P224              := 0;
        PROF.P225              := 0;
        PROF.P226              := 0;
        PROF.P227              := 0;
        PROF.P228              := 0;
        PROF.P229              := 0;
        PROF.P230              := 0;
        PROF.P231              := 0;
        PROF.P232              := 0;
        PROF.P233              := 0;
        PROF.P234              := 0;
        PROF.P235              := 0;
        PROF.P236              := 0;
        PROF.P237              := 0;
        PROF.P238              := 0;
        PROF.P239              := 0;
        PROF.P240              := 0;
        PROF.P241              := 0;
        PROF.P242              := 0;
        PROF.P243              := 0;
        PROF.P244              := 0;
        PROF.P245              := 0;
        PROF.P246              := 0;
        PROF.P247              := 0;
        PROF.P248              := 0;
        PROF.P249              := 0;
        PROF.P250              := 0;
        PROF.P251              := 0;
        PROF.P252              := 0;
        PROF.P253              := 0;
        PROF.P254              := 0;
        PROF.P255              := 0;
        PROF.P256              := 0;
        PROF.P257              := 0;
        PROF.P258              := 0;
        PROF.P259              := 0;
        PROF.P260              := 0;

        IF((CURRENT_DATE - CARD_OPNDATE) > 90.0) THEN
            FPP_START_DATE := CURRENT_DATE - 90.0;
        ELSE
            FPP_START_DATE := CARD_OPNDATE;

			START_DATE_98S := FPP_START_DATE;

        FPP_RECENT_UPDATE_DATE := FPP_START_DATE;
        FPP_RECENT_UPDATE_TIME := 0.0;

        RET_VAL := 1;
    END;

    RETURN(RET_VAL);
ENDRULE;

