#ifndef DSL_H_INCLUDED
#define DSL_H_INCLUDED

#define nextToken tokens[tokenId+1]
#define prevToken tokens[tokenId-1]
#define curToken tokens[tokenId]

#define curTokenType tokens[tokenId]->type
#define curSpecType tokens[tokenId]->value->langType

#define NEW_SPEC_NODE(tokens, nodeL, nodeR) CreateNode(tokens[tokenId]->type, tokens[tokenId]->value->langType, 0.0, NULL, nodeL, nodeR)
#define NEW_VAR_NODE(tokens, nodeL, nodeR) CreateNode(tokens[tokenId]->type, 0.0, 0.0, tokens[tokenId]->value->Name, nodeL, nodeR)


#endif