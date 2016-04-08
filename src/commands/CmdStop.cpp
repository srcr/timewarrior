////////////////////////////////////////////////////////////////////////////////
//
// Copyright 2015 - 2016, Paul Beckingham, Federico Hernandez.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// http://www.opensource.org/licenses/mit-license.php
//
////////////////////////////////////////////////////////////////////////////////

#include <cmake.h>
#include <commands.h>
#include <timew.h>
#include <Interval.h>
#include <Lexer.h>
#include <iostream>

////////////////////////////////////////////////////////////////////////////////
int CmdStop (
  const CLI& cli,
  Rules& rules,
  Database& database)
{
  // Load the most recent interval.
  auto latest = database.getLatestInterval ();

  // Verify the interval is open.
  if (  latest.isStarted () &&
      ! latest.isEnded ())
  {
    // Stop it.
    latest.end (Datetime ());
    database.modifyInterval (latest);

    // User feedback.
    if (rules.getBoolean ("verbose"))
      std::cout << intervalSummarize (rules, latest);

    // If tags were specified, and after removing those tags, there are still
    // tags remaining, then add a contiguous interval.
    auto words = cli.getWords ();
    if (words.size ())
      for (auto& word : cli.getWords ())
        latest.untag (Lexer::dequote (word));

    if (words.size () &&
        latest.tags ().size ())
    {
      // Contiguous with previous interval.
      latest.start (latest.end ());
      latest.end ({0});

      database.addInterval (latest);

      // User feedback.
      if (rules.getBoolean ("verbose"))
        std::cout << "\n" << intervalSummarize (rules, latest);
    }
  }
  else
  {
    std::string message = "There is no active time tracking.";
    if (rules.getBoolean ("verbose"))
      std::cout << message << "\n";
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
