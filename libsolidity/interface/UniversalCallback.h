/*
	This file is part of solidity.

	solidity is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	solidity is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with solidity.  If not, see <http://www.gnu.org/licenses/>.
*/
// SPDX-License-Identifier: GPL-3.0
#pragma once

#include <libsolidity/interface/FileReader.h>
#include <libsolidity/interface/SMTSolverCommand.h>

namespace solidity::frontend
{

class UniversalCallback
{
public:
	UniversalCallback(FileReader _fileReader, SMTSolverCommand _solver) :
		m_fileReader{std::move(_fileReader)},
		m_solver{std::move(_solver)}
	{}

	frontend::ReadCallback::Callback callback()
	{
		return [this](std::string const& _kind, std::string const& _data) -> ReadCallback::Result {
			auto solver = m_solver.solver();
			if (_kind == ReadCallback::kindString(ReadCallback::Kind::ReadFile))
				return m_fileReader.readFile(_kind, _data);
			else if (_kind == ReadCallback::kindString(ReadCallback::Kind::SMTQuery))
				return solver(_kind, _data);
			solAssert(false, "Unknown callback kind.");
		};
	}

private:
	FileReader m_fileReader;
	SMTSolverCommand m_solver;
};

}
