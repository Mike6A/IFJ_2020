#include "scanner.h"
#include "symtable.h"
#include "analyse.h"
#include "semantics.h"
#include "gen_code.h"

int main(){
    ///Tokenizer
    tTokenizer tokenizer;
    initTokenizer(&tokenizer);
    getToken(&tokenizer);
    SyntaxNode* prog = NULL;
    ///Run Syntax Analyse
    prog = getSyntaxGlobal(&tokenizer);
    ///Is Syntax Analyze Correct ?
    if(isError()){
        deleteSyntaxTree(prog);
        freeToken(&tokenizer);
        destructBuilder(&tokenizer.sb);
        return (getError());
    }
    ///Scope
    tScope scope;
    initScope(&scope);
    createScope(&scope);
    ///Run Semantic Analyse
    long returnCode = runSemanticAnalyze(prog, &scope);
    ///IS Semantic Analyse Correct?
    if(returnCode == 0){
        if (getErrorList() != 99)
            ///Print GenCode
            printAndDeleteGenCode(&scope);
    }
    ///Delete everything needed
    deleteIdListGlobal();
    deletepList();
    removeLastLocalScope(&scope);
    deleteSyntaxTree(prog);
    destructBuilder(&tokenizer.sb);
    ///Exit program :)
    return returnCode;
}
