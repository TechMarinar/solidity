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
#include <libsolidity/interface/SMTSolverCommand.h>

#include <liblangutil/Exceptions.h>

#include <libsolutil/CommonIO.h>
#include <libsolutil/Exceptions.h>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/process.hpp>

using solidity::frontend::ReadCallback;
using solidity::langutil::InternalCompilerError;
using solidity::util::errinfo_comment;

using namespace std;

namespace solidity::frontend
{

SMTSolverCommand::SMTSolverCommand(string _solverCmd) : m_solverCmd(_solverCmd) {}

ReadCallback::Result SMTSolverCommand::solve(string const& _kind, string const& _query)
{
	try
	{
		if (_kind != ReadCallback::kindString(ReadCallback::Kind::SMTQuery))
			solAssert(false, "SMTQuery callback used as callback kind " + _kind);

		auto tempDir = boost::filesystem::temp_directory_path();
		auto queryFileName = tempDir / "query.smt2";

		auto queryFile = boost::filesystem::ofstream(queryFileName);
		queryFile << _query;

		auto eldBin = boost::process::search_path("eld");

		if (eldBin.empty())
			return ReadCallback::Result{false, "Eldarica binary not found."};

		boost::process::ipstream pipe;
		boost::process::child eld(
			eldBin,
			"-ssol",
			"-scex",
			queryFileName,
			boost::process::std_out > pipe
		);

		vector<string> data;
		string line;
		while (eld.running() && std::getline(pipe, line))
			if (!line.empty())
				data.push_back(line);

		eld.wait();

		return ReadCallback::Result{true, boost::join(data, "\n")};
	}
	catch (...)
	{
		return ReadCallback::Result{false, "Unknown exception in SMTQuery callback: " + boost::current_exception_diagnostic_information()};
	}
}

}
