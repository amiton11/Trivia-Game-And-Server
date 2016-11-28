#define MAX_USERS 40
#define SERVER_PORT 8820

#define DATABASE_NAME "trivia.db"


#define REQ_SIGN_IN 200
#define REQ_SIGN_OUT 201
#define REQ_SIGN_UP 203

#define REQ_GET_ROOM_LIST 205
#define REQ_GET_USERS_IN_ROOM 207

#define REQ_JOIN_ROOM 209
#define REQ_LEAVE_ROOM 211
#define REQ_CREATE_ROOM 213
#define REQ_CLOSE_ROOM 215

#define REQ_START_GAME 217
#define REQ_SENDING_ANSWER 219 //its not really a request
#define REQ_LEAVE_GAME 222

#define REQ_BEST_SCORES 223
#define REQ_MY_STATUS 225

#define REQ_EXIT_APP 299 



#define P_SIGN_IN_SUCCESS "1020"
#define P_SIGN_IN_FAILED "1021"
#define P_SIGN_IN_ALREADY_CONNECTED "1022"

#define P_SIGN_UP_SUCCESS "1040"
#define P_SIGN_UP_PASS_ILLEGAL "1041"
#define P_SIGN_UP_USER_EXISTS "1042"
#define P_SIGN_UP_USER_ILLEGAL "1043"
#define P_SIGN_UP_OTHER "1044"

#define P_SEND_ROOMS "106"

#define P_SEND_USERS_IN_ROOM "108"

#define P_JOIN_ROOM_SUCCESS "1100"
#define P_JOIN_ROOM_ROOM_FULL "1101"
#define P_JOIN_ROOM_NOT_EXISTS "1102"

#define P_LEAVE_ROOM_SUCCESS "1120"

#define P_CREATE_ROOM_SUCCESS "1140"
#define P_CREATE_ROOM_FAILED "1141"

#define P_CREATE_GAME_FAILED "1180"

#define P_CLOSE_ROOM "116"

#define P_SEND_QUESTION "118"

#define P_ANSWER_RIGHT "1201"
#define P_ANSWER_WRONG "1200"

#define P_END_GAME "121"

#define P_BEST_SCORE "124"
#define P_STATUS "126"
