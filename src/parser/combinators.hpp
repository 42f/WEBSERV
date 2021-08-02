//
// Created by alena on 17/06/2021.
//

#ifndef WEBSERV_COMBINATORS_HPP
#define WEBSERV_COMBINATORS_HPP

#include "bytes/Alpha.hpp"
#include "bytes/Char.hpp"
#include "bytes/Eof.hpp"
#include "bytes/HexChar.hpp"
#include "bytes/Match.hpp"
#include "bytes/Digit.hpp"
#include "bytes/Itag.hpp"
#include "bytes/Tag.hpp"
#include "bytes/OneOf.hpp"
#include "bytes/TakeUntil.hpp"
#include "bytes/TakeExact.hpp"
#include "bytes/TakeUntilMatch.hpp"
#include "bytes/TakeWhile.hpp"
#include "bytes/TakeWith.hpp"
#include "bytes/Strip.hpp"

#include "combinators/Alt.hpp"
#include "combinators/AsSlice.hpp"
#include "combinators/Map.hpp"
#include "combinators/Opt.hpp"
#include "combinators/Wrap.hpp"

#include "sequence/Delimited.hpp"
#include "sequence/Many.hpp"
#include "sequence/Preceded.hpp"
#include "sequence/Separated.hpp"
#include "sequence/Terminated.hpp"
#include "sequence/Sequence.hpp"

#include "Error.hpp"

#endif //WEBSERV_COMBINATORS_HPP
