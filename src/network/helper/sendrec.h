/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef SENDREC__H
#define SENDREC__H

struct sockaddr_in;

namespace message { struct SMessage; }

int MySend(int sock, const message::SMessage& msg, const sockaddr_in& addr);
int MyReceive(int sock, message::SMessage& msg);

#endif // SENDREC__H
