import os

files = [
    r"include\slc\formats\v3\atom.hpp",
    r"include\slc\formats\v3\builtin.hpp",
    r"include\slc\formats\v3\replay.hpp"
]

replacements = {
    "auto result = TRY(Serializer::read(in));": "auto _res = Serializer::read(in);\n      if (!_res) return std::unexpected(_res.error());\n      auto result = *_res;",
    "auto section = TRY(Section::special(actions[i]));": "auto _sec = Section::special(actions[i]);\n        if (!_sec) return std::unexpected(_sec.error());\n        auto section = *_sec;",
    "TRY(ActionAtom::prepareSections(m_actions, sections));": "{ auto _prep = ActionAtom::prepareSections(m_actions, sections);\n    if (!_prep) return std::unexpected(_prep.error()); }",
    "TRY(replay.m_atoms.readAll(in));": "{ auto _read = replay.m_atoms.readAll(in);\n    if (!_read) return std::unexpected(_read.error()); }"
}

for filepath in files:
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    for old, new in replacements.items():
        content = content.replace(old, new)
        
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)
print("Files patched.")
