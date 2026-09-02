#ifndef EDITRECORD_H
#define EDITRECORD_H

#include <string>

enum class EditType {
    INSERT,
    DELETE,
    REPLACE
};

struct EditRecord {
    EditType type;
    int lineNumber; // 1-based index
    std::string oldContent;
    std::string newContent;

    EditRecord() : type(EditType::INSERT), lineNumber(1) {}
    EditRecord(EditType t, int line, const std::string& oldText, const std::string& newText)
        : type(t), lineNumber(line), oldContent(oldText), newContent(newText) {}
};

#endif // EDITRECORD_H
