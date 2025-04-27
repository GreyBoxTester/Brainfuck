#include "Optimizer.h"

Optimizer::Optimizer() {}

/*
Plan:
step 1:
	remove [[...]] and same
	remove [] (maybe)
	make top loop identifiers
step 2:
	optimize loops and store result instructions in optimized instruction vector

	ADD 13
	ADDMUL 1 2
	ADDMUL 4 5
	ADDMUL 5 2
	ADDMUL 6 1
	SET0

	to 



	ADD 13
	LDCUR
	STORE 6
	SHL 1
	STORE 1
	STORE 5
	LDCUR
	MUL 5
	STORE 4

	ADDMUL 1 2
	ADDMUL 4 5
	ADDMUL 5 2
	ADDMUL 6 1
	SET0

step 3:
	order optimizations ?
	instruction optimizations:
		set0
		loops
		...         =>  set0
		loope

		loops
		...			 ...
		set0     =>  set0
		loope

		set0
		addmul ...  =>  set0
		addmul

		set0
		set0		=> set0
		set0

		mabye add:
		[-]+++[->+++<] => set val
	*/
void Optimizer::optimize(std::vector<Instruction>* instructionCode) const
{
	std::vector<LoopInfo> loopsInfo;
	makeLoopsInfo(instructionCode, &loopsInfo);

	std::vector<Instruction> optimized;
	optimized.reserve(instructionCode->size());

	auto itStart = instructionCode->begin();

	for (const LoopInfo& loopInfo : loopsInfo)
	{
		auto loopInstStart = loopInfo.start + 1;
		auto loopInstEnd = loopInfo.end - 1;
		size_t loopLen = loopInstEnd - loopInstStart;

		if (loopLen == 1)
		{
			optimized.insert(optimized.end(), itStart, loopInfo.start);

			if (loopInstStart->code == ADD && llabs((int64_t)loopInstStart->data0) == 1)
			{
				optimized.emplace_back(SET0);
			}
			else if (loopInfo.movCount == 1)
			{
				optimized.emplace_back(SRCH0, SEARCH_RIGHT);
			}
			else if (loopInfo.movCount == -1)
			{
				optimized.emplace_back(SRCH0, SEARCH_LEFT);
			}
			else
			{
				optimized.insert(optimized.end(), loopInfo.start, loopInfo.end);
			}

			itStart = loopInfo.end;
		}
		else if (loopLen > 1)
		{
			optimized.insert(optimized.end(), itStart, loopInfo.start);

			bool sfIsIncDec = loopInstStart->code == ADD && llabs((int64_t)loopInstStart->data0) == 1;
			bool efIsIncDec = loopInstEnd->code == ADD && llabs((int64_t)loopInstEnd->data0) == 1;

			if ((sfIsIncDec ^ efIsIncDec) && loopInfo.movCount == 0) //multiplication loop
			{
				if ((sfIsIncDec && loopInstStart->data0 == 1) || (efIsIncDec && loopInstEnd->data0 == 1)) { optimized.emplace_back(NEG); }

				int64_t offset = 0;

				for (auto it = loopInstStart + sfIsIncDec; it < loopInstEnd - efIsIncDec; ++it)
				{
					if (it->code == ADD)
					{
						optimized.emplace_back(ADDMUL, it->data0, offset);
					}
					else if (it->code == MOV)
					{
						offset += it->data0;
					}
				}

				optimized.emplace_back(SET0);
			}
			else
			{
				optimized.insert(optimized.end(), loopInfo.start, loopInfo.end);
			}

			itStart = loopInfo.end;
		}
	}

	optimized.insert(optimized.end(), itStart, instructionCode->end());

	instructionCode->swap(optimized);
}

void Optimizer::makeLoopsInfo(std::vector<Instruction>* instructionCode, std::vector<LoopInfo>* loopsInfo) const
{
	int64_t movCount = 0;
	bool hasIO = false;
	bool isTopLoop = false;

	auto itStart = instructionCode->begin();

	for (auto it = instructionCode->begin(); it < instructionCode->end(); ++it)
	{
		if (it->code == MOV)
		{
			movCount += it->data0;
		}
		if (it->code == INP || it->code == OTP)
		{
			hasIO = true;
		}
		else if (it->code == LOOPS)
		{
			itStart = it;
			movCount = 0;
			hasIO = false;
			isTopLoop = true;
		}
		else if (it->code == LOOPE)
		{
			if (isTopLoop && !hasIO)
			{
				loopsInfo->emplace_back(itStart, it + 1, movCount);
				isTopLoop = false;
			}
		}
	}
}

/*auto loopInstStart = loopInfo.start + 1;
		auto loopInstEnd = loopInfo.end - 1;
		size_t loopLen = loopInstEnd - loopInstStart;

		if (loopInfo.movCount == 0)
		{
			optimized.insert(optimized.end(), itStart, loopInfo.start);

			bool sfIsIncDec = loopInstStart->code == ADD && llabs((int64_t)loopInstStart->data0) == 1;
			bool efIsIncDec = loopInstEnd->code == ADD && llabs((int64_t)loopInstEnd->data0) == 1;

			if (loopLen == 1 && sfIsIncDec) // is [+] or [-]
			{
				optimized.emplace_back(SET0);
			}
			else if (loopLen > 1 && (sfIsIncDec ^ efIsIncDec)) //multiplication loop
			{
				if ((sfIsIncDec && loopInstStart->data0 == 1) || (efIsIncDec && loopInstEnd->data0 == 1)) { optimized.emplace_back(NEG); }

				int64_t offset = 0;

				for (auto it = loopInstStart + sfIsIncDec; it < loopInstEnd - efIsIncDec; ++it)
				{
					if (it->code == ADD)
					{
						optimized.emplace_back(ADDMUL, it->data0, offset);
					}
					else if (it->code == MOV)
					{
						offset += it->data0;
					}
				}

				optimized.emplace_back(SET0);
			}
			else
			{
				optimized.insert(optimized.end(), loopInfo.start, loopInfo.end);
			}

			itStart = loopInfo.end;
		}
		else
		{
			optimized.insert(optimized.end(), itStart, loopInfo.start);

			if (loopInfo.movCount == 1)
			{
				optimized.emplace_back(SRCH0, SEARCH_RIGHT);
			}
			else if (loopInfo.movCount == -1)
			{
				optimized.emplace_back(SRCH0, SEARCH_LEFT);
			}
			else
			{
				optimized.insert(optimized.end(), loopInfo.start, loopInfo.end);

			}

			itStart = loopInfo.end;
		}*/


/*void Optimizer::removeUnneccessaryBrackets(std::vector<Instruction>* instructionCode) const
{
	std::vector<Instruction> newCode;
	newCode.reserve(instructionCode->size());

	std::vector<std::vector<Instruction>::iterator> loopsStart;

	auto itLastInsert = instructionCode->begin();

	for (auto it = instructionCode->begin(); it < instructionCode->end(); ++it)
	{
		if (it->code == LOOPS)
		{
			if ((it + 1)->code != LOOPE) //if (it + 1 < instructionCode->end()) {  } code should be with no missing brackets
			{ 
				loopsStart.push_back(it); 
				it++;
			}
		}
		else if (it->code == LOOPE)
		{
			if (it + 1 < instructionCode->end() && loopsStart.size() > 1)
			{
				if (!((it + 1)->code == LOOPE && loopsStart.back() - loopsStart[loopsStart.size() - 2] == 1))
				{

				}
			}
			else
			{

			}

			loopsStart.pop_back();
		}
	}

	instructionCode->swap(newCode);
}*/

/*std::vector<Instruction> optimized;
	optimized.reserve(instructionCode->size());

	std::vector<std::vector<Instruction>::iterator> loopsStart;
	auto itLastIns = instructionCode->begin();
	int64_t movCount = 0;
	bool loopHasIO = false;

	for (auto it = instructionCode->begin(); it < instructionCode->end(); ++it)
	{
		if (it->code == LOOPS)
		{
			optimized.insert(optimized.end(), itLastIns, it);
			itLastIns = it;
			movCount = 0;
			loopHasIO = false;
			loopsStart.emplace_back(it);
		}
		else if (it->code == LOOPE)
		{
			auto itLoopStart = loopsStart.back() + 1;
			loopsStart.pop_back();
			auto itLoopEnd = it - 1;
			size_t loopLen = itLoopEnd - itLoopStart + 1;
			bool sfIsIncDec = itLoopStart->code == ADD && (itLoopStart->data0 == 1 || itLoopStart->data0 == (size_t)(-1));
			bool efIsIncDec = itLoopEnd->code == ADD && (itLoopEnd->data0 == 1 || itLoopEnd->data0 == (size_t)(-1));

			if (loopLen == 1 && sfIsIncDec)
			{
				optimized.emplace_back(SET0);
				itLastIns = it + 1;
			}
			else if (!loopHasIO && loopLen > 1 && movCount == 0 && (sfIsIncDec ^ efIsIncDec))
			{
				if ((sfIsIncDec && itLoopStart->data0 == 1) || (efIsIncDec && itLoopEnd->data0 == 1))
				{
					optimized.emplace_back(NEG);
				}

				for (auto it1 = itLoopStart + sfIsIncDec; it1 < it - efIsIncDec; ++it1)
				{
					if (it1->code == ADD)
					{
						optimized.emplace_back(ADDMUL, movCount, it1->data0);
					}
					else if (it1->code == MOV)
					{
						movCount += it1->data0;
					}
				}

				optimized.emplace_back(SET0);
				itLastIns = it + 1;
			}
			else
			{
				optimized.insert(optimized.end(), itLastIns, it);
				itLastIns = it;
			}
		}
		else if (it->code == MOV)
		{
			movCount += it->data0;
		}
		else if (it->code == INP || it->code == OTP)
		{
			loopHasIO = true;
		}
	}

	optimized.insert(optimized.end(), itLastIns, instructionCode->end());
	instructionCode->swap(optimized);*/

	/*std::vector<Instruction> optimized;
	optimized.reserve(instructionCode->size());

	std::stack<std::vector<Instruction>::iterator, std::vector<std::vector<Instruction>::iterator>> loopStartIter;

	size_t loopMovCount = 0;
	auto itStart = instructionCode->begin();
	auto it = instructionCode->begin();

	for (; it < instructionCode->end(); ++it)
	{
		if (it->code == LOOPS)
		{
			optimized.insert(optimized.end(), itStart, it);
			itStart = it;
			loopStartIter.push(it);
			loopMovCount = 0;
		}
		else if (it->code == LOOPE)
		{
			auto itLoopStart = loopStartIter.top();
			loopStartIter.pop();
			size_t loopLen = it - itLoopStart - 1;
			Instruction& loopStartInstruction = *(itLoopStart + 1);
			Instruction& loopEndInstruction = *(it - 1);
			bool startInstructionIsIncDec = loopStartInstruction.code == ADD && (loopStartInstruction.data0 == 1 || loopStartInstruction.data0 == (size_t)(-1));
			bool endInstructionIsIncDec = loopEndInstruction.code == ADD && (loopEndInstruction.data0 == 1 || loopEndInstruction.data0 == (size_t)(-1));

			if (loopLen == 1)
			{
				if (startInstructionIsIncDec)
				{
					itStart = it + 1;
					optimized.push_back({ SET0 });
				}
			}
			else if (loopMovCount == 0 && loopLen > 1 && (startInstructionIsIncDec ^ endInstructionIsIncDec))
			{
				itStart = it + 1;
				//reuse loopMovCount
				for (auto itCurrent = itLoopStart + 1 + startInstructionIsIncDec; itCurrent < it - endInstructionIsIncDec; ++itCurrent)
				{
					if (itCurrent->code == ADD)
					{
						optimized.push_back({ ADDMUL, loopMovCount, itCurrent->data0 });
					}
					else if (itCurrent->code == MOV)
					{
						loopMovCount += itCurrent->data0;
					}
				}

				optimized.push_back({ SET0 });
			}
			else
			{
				optimized.insert(optimized.end(), itStart, it);
				itStart = it;
			}
		}
		else if (it->code == MOV)
		{
			loopMovCount += it->data0;
		}
	}

	optimized.insert(optimized.end(), itStart, it);

	instructionCode->swap(optimized);*/
