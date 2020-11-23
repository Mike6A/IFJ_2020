/**
 * @file gen_code.h
 *
 * @brief Header file for code generator
 *
 * @author Michal Mikota (xmikot01@stud.fit.vutbr.cz)
 */

//only for TESTING PURPOSE in this file
typedef enum { TInt, TDouble, TString, TBool, TFunc } TItem;
 TItem type;

 void declared_vars_default_init(TItem type)
{

    switch(type)
    {

        case TInt:
                printf("int@0\n");
                break;

        case TDouble: //ASK: why not TFloat
                printf("float@0.0\n");
                break;

        case TString:
                printf("string@\n");
                break;

        case TBool:
                printf("bool@false\n");
                break;
        
    }


}


