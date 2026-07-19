/*****************************************************************
 * Copyright (C) 2017 Robert Valler - All rights reserved.
 *
 * This file is part of the project: <insert project name here>
 *
 * This project can not be copied and/or distributed
 * without the express permission of the copyright holder
 *****************************************************************/

#ifndef CHECKSUM__H
#define CHECKSUM__H

#include <vector>
#include <cstdint>

bool CheckSumGenerate(std::vector<std::uint8_t>& data);
bool CheckSumValidate(std::vector<std::uint8_t>& data);

#endif // CHECKSUM__H
