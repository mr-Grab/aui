//
// Created by alex2772 on 11/6/20.
//

#include "Pack.h"
#include <AUI/Crypt/AHash.h>
#include <AUI/IO/APath.h>
#include <AUI/IO/FileInputStream.h>
#include <AUI/IO/FileOutputStream.h>
#include <AUI/Util/ATokenizer.h>
#include <AUI/Util/LZ.h>

void Pack::run(Toolbox& t) {
    if (t.args.size() != 2)
    {
        throw IllegalArgumentsException("invalid argument count");
    }
    else
    {
        try {
            APath p(t.args[1]);
            p.removeFileRecursive();
            p.makeDirs();
        } catch (...) {

        }
        unsigned index = 0;
        try
        {
            for (auto& entry : APath(t.args[0]).listDir(LF_REGULAR_FILES | LF_RECURSIVE))
            {
                ++index;
                try
                {
                    AByteBuffer buffer;
                    AString filePath = entry;
                    filePath = filePath.mid(t.args[0].length() + 1);
                    filePath = filePath.replaceAll("\\", '/');

                    buffer << filePath.toStdString();
                    auto fis = _new<FileInputStream>(entry);

                    AByteBuffer data;

                    char buf[0x1000];
                    for (int r; (r = fis->read(buf, sizeof(buf))) > 0;)
                    {
                        data.put(buf, r);
                    }

                    uint32_t nameHash = std::hash<AString>()(t.args[0] + t.args[1]) ^index;

                    auto path = APath(t.args[1])
                            .file(("assets" + AString::number(nameHash) + ".cpp"));

                    AString name(path);

                    auto fileHash = AHash::sha512(data).toHexString();

                    // попробуем открыть cpp файл по этому пути. если он существует, то есть шанс, что
                    // нам не придётся перезаписывать одно и то же содержимое файла.
                    try {
                        ATokenizer t(_new<FileInputStream>(name));

                        // пропускаем первую строку, где написано предупреждение, что этот файл
                        // сгенерирован автоматически.
                        t.readStringUntilUnescaped('\n');

                        // попробуем прочитать хеш со строчки. он должен начинаться с комментария.
                        AString desiredBeginning = "// hash: ";
                        AString actualBeginning = t.readString(desiredBeginning.length());
                        if (actualBeginning == desiredBeginning) {
                            // наш клиент.
                            auto targetFileHash = t.readStringUntilUnescaped('\n');
                            if (targetFileHash == fileHash) {
                                std::cout << "skipped " << filePath << std::endl;
                                continue;
                            }
                        }
                    } catch (...) {

                    }


                    buffer << uint32_t(data.getSize());
                    buffer << data;

                    AByteBuffer packed;
                    LZ::compress(buffer, packed);



                    auto out = _new<FileOutputStream>(name);
                    *out << "// This file is autogenerated by AUI.Toolkit. Please do not modify.\n"
                            "// hash: "_as << fileHash << "\n"
                                                          "\n"
                                                          "#include \"AUI/Common/AByteBuffer.h\"\n"
                                                          "#include \"AUI/Util/BuiltinFiles.h\"\n"
                                                          "const static unsigned char AUI_PACKED_asset"_as << AString::number(nameHash)
                         << "[] = {"_as;
                    for (uint8_t c : packed) {
                        char buf[32];
                        sprintf(buf, "0x%02x,", c);
                        *out << AString(buf);
                    }
                    *out << "};\n"_as;

                    *out << "struct Assets"_as << AString::number(nameHash) << " {\n"_as
                         << "\tAssets"_as + AString::number(nameHash) + "(){\n"_as
                                 "\t\tBuiltinFiles::load(AUI_PACKED_asset"_as
                         << AString::number(nameHash)
                         << ", sizeof(AUI_PACKED_asset"_as
                         << AString::number(nameHash) << "));\n\t}\n};\n"
                                                         "static Assets"_as
                         << AString::number(nameHash)
                         << " a"_as << AString::number(nameHash) << ";"_as;
                    out = nullptr;
                    std::cout << filePath << " -> " << path
                              << std::endl;
                }
                catch (std::exception& e)
                {
                    std::cout << "Warning: could not read file " << entry << ": " << e.what() << std::endl;
                }
                catch (AException& e)
                {
                    std::cout << "Warning: could not read file " << entry << ": " << e.getMessage().toStdString() << std::endl;
                }
            }

        }
        catch (...)
        {
            std::cout << "Could not open directory: " << APath(t.args[0]).absolute() << std::endl;
            exit(-2);
        }
    }
}

AString Pack::getName() {
    return "pack";
}

AString Pack::getSignature() {
    return "<dir> <dst>";
}

AString Pack::getDescription() {
    return "pack a dir into the .h file";
}