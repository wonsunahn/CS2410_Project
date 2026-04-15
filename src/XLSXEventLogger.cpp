#include "XLSXEventLogger.h"

#include <stdexcept>

XLSXEventLogger::XLSXEventLogger(const std::string& outputPath)
	: worksheetName("Sheet1"), nextRow(2), currentCycle(0), open(false)
{
	document.create(outputPath, true);
	auto worksheet = document.workbook().worksheet(worksheetName);
	worksheet.cell("A1").value() = "Seq";
	worksheet.cell("B1").value() = "PC";
	worksheet.cell("C1").value() = "Instruction";
	open = true;
}

XLSXEventLogger::~XLSXEventLogger()
{
	close();
}

void XLSXEventLogger::startCycle()
{
	if (!open)
		return;

	currentCycle++;

	// Cycles are contiguous and 1-based, so column is deterministic:
	// cycle 1 -> col 4 (D), cycle 2 -> col 5 (E), ...
	int col = currentCycle + 3;
	auto worksheet = document.workbook().worksheet(worksheetName);
	worksheet.cell(OpenXLSX::XLCellReference(1, col)).value() = currentCycle;
}

int XLSXEventLogger::ensureInstructionRow(int sequence, int pc, const std::string& instruction)
{
	auto existing = instructionRow.find(sequence);
	if (existing != instructionRow.end())
		return existing->second;

	int row = nextRow++;
	instructionRow[sequence] = row;
	auto worksheet = document.workbook().worksheet(worksheetName);
	worksheet.cell(OpenXLSX::XLCellReference(row, 1)).value() = sequence;
	worksheet.cell(OpenXLSX::XLCellReference(row, 2)).value() = pc;
	worksheet.cell(OpenXLSX::XLCellReference(row, 3)).value() = instruction;
	return row;
}

std::string XLSXEventLogger::readCellAsString(int row, int col)
{
	auto worksheet = document.workbook().worksheet(worksheetName);

	try
	{
		return worksheet.cell(OpenXLSX::XLCellReference(row, col)).value().get<std::string>();
	}
	catch (const std::exception&)
	{
		return "";
	}
}

void XLSXEventLogger::logFetchedInstruction(int sequence, int pc, const std::string& instruction)
{
	if (!open)
		return;

	if (currentCycle <= 0)
		return;

	ensureInstructionRow(sequence, pc, instruction);
	markStage(sequence, "IF");
}

void XLSXEventLogger::markStage(int sequence, const std::string& stage)
{
	if (!open)
		return;

	if (currentCycle <= 0)
		return;

	if (instructionRow.count(sequence) == 0)
		return;

	int row = instructionRow[sequence];
	int col = currentCycle + 3;
	auto worksheet = document.workbook().worksheet(worksheetName);
	assert (readCellAsString(row, col).empty() || readCellAsString(row, col) == stage); // Ensure we don't overwrite an existing stage for this cycle
	worksheet.cell(OpenXLSX::XLCellReference(row, col)).value() = stage;
}

void XLSXEventLogger::close()
{
	if (!open)
		return;

	auto worksheet = document.workbook().worksheet(worksheetName);
	int legendRow = nextRow + 1;
	worksheet.cell(OpenXLSX::XLCellReference(legendRow, 1)).value() = "[Legend]";

	legendRow++;
	worksheet.cell(OpenXLSX::XLCellReference(legendRow, 1)).value() = "IF";
	worksheet.cell(OpenXLSX::XLCellReference(legendRow, 2)).value() = "Instruction Fetch";

	legendRow++;
	worksheet.cell(OpenXLSX::XLCellReference(legendRow, 1)).value() = "ID";
	worksheet.cell(OpenXLSX::XLCellReference(legendRow, 2)).value() = "Instruction Decode";

	legendRow++;
	worksheet.cell(OpenXLSX::XLCellReference(legendRow, 1)).value() = "IS/EX";
	worksheet.cell(OpenXLSX::XLCellReference(legendRow, 2)).value() = "Add to Issue Queue/Execute (if instruction is ready, it can start executing in the same cycle it is added to issue queue)";

	legendRow++;
	worksheet.cell(OpenXLSX::XLCellReference(legendRow, 1)).value() = "EX";
	worksheet.cell(OpenXLSX::XLCellReference(legendRow, 2)).value() = "Execute";

	legendRow++;
	worksheet.cell(OpenXLSX::XLCellReference(legendRow, 1)).value() = "WB/CT";
	worksheet.cell(OpenXLSX::XLCellReference(legendRow, 2)).value() = "Write Back/Commit (instruction can be committed on the same cycle it writes back)";

	document.save();
	document.close();
	open = false;
}
