// The "easy translate" library, in c++.
//
// Webs: https://github.com/JaderoChan/easy_translate
// You can contact me by email: c_dl_cn@outlook.com

// GPL-3.0 License
//
// Copyright (c) 2024 頔珞JaderoChan

#ifndef EASY_TRANSLATE_HPP
#define EASY_TRANSLATE_HPP

#include <cstddef>              // size_t
#include <string>               // string
#include <vector>               // vector
#include <set>                  // set
#include <map>                  // map
#include <fstream>              // ifstream

#include <nlohmann/json.hpp>    // json

/// @brief Define this macro to enable easytr::updateTranslationsFiles() function.
/// @note If you define this macro, the easytr::TranslateManager::translate() function will store
/// all `Text ID` to memory used for possible update the `Translations file`s.
// #define EASY_TRANSLATE_DUMP_TEXTID

// - Translate function
//   - Usage: EASYTR("Text ID")
//   - Return the `Translation text` corresponding given `Text ID`.
#define EASYTR(x) easytr::tr(x)

// The following is a sample directory structure and content structure for
// the `Languages file` and `Translations file`:
//
// - languages.json (Languages file)
//   - en_US (Language ID) : en_US.json (Translations filename)
//   - zh_CN (Language ID) : zh_CN.json (Translations filename)
//   - ...
//   - ja_JP (Language ID) : ja_JP.json (Translations filename)
//   - fr_FR (Language ID) : fr_FR.json (Translations filename)
//
// - en_US.json (Translations file)
//   - App.Title  (Text ID) : Easy Translation (Translation text)
//   - App.Author (Text ID) : JaderoChan       (Translation text)
//   - ...
//
// - zh_CN.json
//   - App.Title  (Text ID) : 轻松翻译 (Translation text)
//   - App.Author (Text ID) : 頔珞     (Translation text)
//   - ...

// - Languages
//   - Language ID : Translations filename
//   - ...
//
// - Translations
//   - Text ID : Translation text
//   -...

// Note:
// The `Languages file` (e.g. languages.json) and the `Transalations file` (e.g. en.json, zh.json) should
// use the UTF-8 encoding to save.

namespace easytr
{

class Languages
{
    friend class TranslateManager;
public:
    Languages() = default;

    Languages(const std::vector<std::pair<std::string, std::string>>& langs)
    {
        for (const auto& var : langs)
            languages_.insert({ var.first, var.second });
    }

    Languages(const std::map<std::string, std::string>& langs) : languages_(langs) {}

    /// @brief Load the `Languages` from a json string.
    /// @note If the json is invalid, the `Languages` will be empty.
    static Languages fromJson(const std::string& json)
    {
        using Json = nlohmann::json;

        Json j = Json::parse(json, nullptr, false, true);
        if (j.is_discarded())
            return Languages();

        std::map<std::string, std::string> list;
        for (const auto& var : j.items())
            list.insert({ var.key(), var.value() });

        return Languages(list);
    }

    /// @brief Load the `Languages` from a json file.
    /// @note If the json is invalid, the `Languages` will be empty.
    static Languages fromFile(const std::string& filename)
    {
        using Json = nlohmann::json;

        std::ifstream ifs(filename);
        if (!ifs.is_open())
            return Languages();

        Json j = Json::parse(ifs, nullptr, false, true);
        if (j.is_discarded())
        {
            ifs.close();
            return Languages();
        }

        std::map<std::string, std::string> list;
        ifs.close();
        for (const auto& var : j.items())
            list.insert({ var.key(), var.value() });

        return Languages(list);
    }

    /// @brief Get the `Translations filename` of the given `Language ID`.
    std::string at(const std::string& languageId) const
    { return languages_.at(languageId); }

    /// @brief Get the number of the `Language ID`.
    size_t count() const { return languages_.size(); }

    /// @brief Check whether has not any `Language ID`.
    bool empty() const { return count() == 0; }

    /// @brief Check whether exists the given `Language ID`.
    bool has(const std::string& languageId) const
    { return languages_.find(languageId) != languages_.end(); }

    /// @brief Get all `Language ID`s.
    std::vector<std::string> getIds() const
    {
        std::vector<std::string> ids;
        for (const auto& var : languages_)
            ids.push_back(var.first);
        return ids;
    }

    /// @brief Write the `Languages` to a json file.
    /// @return If the failed to write the file return false else return true.
    bool write(const std::string& filename = "languages.json") const
    {
        using Json = nlohmann::json;

        Json j;
        for (const auto& var : languages_)
            j[var.first] = var.second;

        std::ofstream ofs(filename);
        if (!ofs.is_open())
            return false;

        ofs << j.dump(4);
        ofs.close();
        return true;
    }

    /// @brief Add a pair of the `Language ID` and `Translations filename`.
    /// @note If the given `Language ID` already exists, do nothing.
    void add(const std::string& languageId, const std::string& translationsFilename)
    {
        if (!has(languageId))
            languages_.insert({ languageId, translationsFilename });
    }

    /// @brief Remove a `Language ID` and it corresponding `Translations filename`.
    void remove(const std::string& languageId)
    {
        if (has(languageId))
            languages_.erase(languageId);
    }

    /// @brief Remove all `Language ID`s and it corresponding `Translations filename`s.
    void clear() { languages_.clear(); }

private:
    // {Language ID : Translations filename}
    std::map<std::string, std::string> languages_;
};

class Translations
{
    friend class TranslateManager;
public:
    Translations() = default;

    Translations(const std::vector<std::pair<std::string, std::string>>& trans)
    {
        for (const auto& var : trans)
            translations_.insert({ var.first, var.second });
    }

    Translations(const std::map<std::string, std::string>& trans) : translations_(trans) {}

    /// @brief Load the `Translations` from a json string.
    /// @note If the json is invalid, the `Translations` will be empty.
    static Translations fromJson(const std::string& json)
    {
        using Json = nlohmann::json;

        Json j = Json::parse(json, nullptr, false, true);
        if (j.is_discarded())
            return Translations();

        std::map<std::string, std::string> list;
        for (const auto& var : j.items())
            list.insert({ var.key(), var.value() });

        return Translations(list);
    }

    /// @brief Load the `Translations` from a json file.
    /// @note If the json is invalid, the `Translations` will be empty.
    static Translations fromFile(const std::string& filename)
    {
        using Json = nlohmann::json;

        std::ifstream ifs(filename);
        if (!ifs.is_open())
            return Translations();

        Json j = Json::parse(ifs, nullptr, false, true);
        if (j.is_discarded())
        {
            ifs.close();
            return Translations();
        }

        std::map<std::string, std::string> list;
        ifs.close();
        for (const auto& var : j.items())
            list.insert({ var.key(), var.value() });

        return Translations(list);
    }

    /// @brief Get the `Translation text` of the given `Text ID`.
    /// @note If the given `Text ID` is not exist, return the `Text ID` itself.
    const char* at(const std::string& textId) const
    {
        if (!has(textId))
            return textId.c_str();
        return translations_.at(textId).c_str();
    }

    /// @brief Get the number of the `Text ID`.
    size_t count() const { return translations_.size(); }

    /// @brief Check whether has not any `Text ID`.
    bool empty() const { return count() == 0; }

    /// @brief Check whether exists the given `Text ID`.
    bool has(const std::string& textId) const
    { return translations_.find(textId) != translations_.end(); }

    /// @brief Get all `Text ID`s
    std::vector<std::string> getIds() const
    {
        std::vector<std::string> ids;
        for (const auto& var : translations_)
            ids.push_back(var.first);
        return ids;
    }

    /// @brief Write the `Translations` to a json file.
    /// @return If the failed to write the file return false else return true.
    bool write(const std::string& filename) const
    {
        using Json = nlohmann::json;

        Json j;
        for (const auto& var : translations_)
            j[var.first] = var.second;

        std::ofstream ofs(filename);
        if (!ofs.is_open())
            return false;

        ofs << j.dump(4);
        ofs.close();
        return true;
    }

    /// @brief Add a pair of the `Text ID` and `Translation text`.
    /// @note If the given `Text ID` already exists, do nothing.
    void add(const std::string& textId, const std::string& translation)
    {
        if (!has(textId))
            translations_.insert({ textId, translation });
    }

    /// @brief Remove a `Text ID` and it corresponding `Translation text`.
    void remove(const std::string& textId)
    {
        if (has(textId))
            translations_.erase(textId);
    }

    /// @brief Remove all `Text ID`s and it corresponding `Translation text`s.
    void clear() { translations_.clear(); }

private:
    // {Text ID : Translation text}
    std::map<std::string, std::string> translations_;
};

// Singleton class
class TranslateManager
{
public:
    static TranslateManager& getInstance()
    {
        static TranslateManager instance;
        return instance;
    }

    /// @brief Get the `Translation text` of the given `Text ID` on current language.
    /// @note If the given `Text ID` is not exist on the current language, return the `Text ID` itself.
#ifndef EASY_TRANSLATE_DUMP_TEXTID
    const char* translate(const std::string& textId) const
    {
        return translations_.at(textId);
    }
#else
    /// @brief Get the `Translation text` of the given `Text ID` on current language.
    /// @note If the given `Text ID` is not exist on the current language, return the `Text ID` itself.
    const char* translate(const std::string& textId)
    {
        textIds_.insert(textId);
        return translations_.at(textId);
    }
#endif // EASY_TRANSLATE_DUMP_TEXTID

    /// @brief Update all `Translations file`s. (add pairs of the new `Text ID` and empty `Translation text`)
    /// @return The number of updated files.
    /// @note The new `Text ID` is from all `Text ID` that passed as #translate() function argument in programs.
    /// @note This function can help you to easy get the all `Text ID` that need to translate.
    /// @attention Make sure to call this function after you already call all #translate() function,
    /// if not you will get incomplete `Text ID` list.
    /// @attention This function is not effect when undefine the macro \ref EASY_TRANSLATE_DUMP_TEXTID.
    size_t updateTranslationsFiles() const
    {
    #ifndef EASY_TRANSLATE_DUMP_TEXTID
        return 0;
    #else
        using Json = nlohmann::json;

        size_t updated = 0;
        for (const auto& languageId : languages_.getIds())
        {
            std::string filename = languages_.at(languageId);
            std::ifstream ifs(filename);
            Json j;
            if (!ifs.is_open())
            {
                for (const auto& textId : textIds_)
                    j[textId] = "";
            }
            else
            {
                j = Json::parse(ifs, nullptr, false, true);
                if (j.is_discarded())
                {
                    j = Json();
                    for (const auto& textId : textIds_)
                        j[textId] = "";
                }
                else
                {
                    std::map<std::string, std::string> map; // For sort
                    for (const auto& textId : textIds_)
                        j.contains(textId) ? map.insert({ textId, j[textId] }) : map.insert({ textId, "" });

                    j.clear();
                    for (const auto& var : map)
                        j[var.first] = var.second;
                }

                ifs.close();
            }

            std::ofstream ofs(filename);
            if (!ofs.is_open())
                continue;

            ofs << j.dump(4);
            ofs.close();
            updated++;
        }

        return updated;
    #endif // EASY_TRANSLATE_DUMP_TEXTID
    }

    /// @brief Set the `Languages`.
    void setLanguages(const Languages& languages) { languages_ = languages; }

    /// @brief Set the `Languages` that from a json file.
    void setLanguages(const std::string& filename) { languages_ = Languages::fromFile(filename); }

    /// @brief Set the current language by `Language ID`.
    /// @return If success to change return true else return false.
    bool setCurrentLanguage(const std::string& languageId)
    {
        if (!hasLanguage(languageId))
            return false;

    #ifdef EASY_TRANSLATE_DUMP_TEXTID
        bool isFirst = currentLanguage_.empty();
    #endif // !EASY_TRANSLATE_DUMP_TEXTID
        currentLanguage_ = languageId;
        translations_ = Translations::fromFile(languages_.at(languageId));

    #ifdef EASY_TRANSLATE_DUMP_TEXTID
        if (isFirst)
        {
            for (const auto& var : translations_.translations_)
                textIds_.insert(var.first);
        }
    #endif // !EASY_TRANSLATE_DUMP_TEXTID

        return true;
    }

    /// @brief Get the number of the `Language ID`.
    size_t languageCount() const { return languages_.count(); }

    /// @brief Get the number of the `Text ID` on current language.
    size_t textCount() const { return translations_.count(); }

    /// @brief Check whether exists the given `Language ID`.
    bool hasLanguage(const std::string& languageId) const { return languages_.has(languageId); }

    /// @brief Check whether exists the given `Text ID`.
    bool hasText(const std::string& textId) const { return translations_.has(textId); }

    /// @brief Get the `Language ID` of the current language.
    const char* currentLanguage() const { return currentLanguage_.c_str(); }

    const Languages& languages() const { return languages_; }

    const Translations& translations() const { return translations_; }

private:
    TranslateManager() = default;

    ~TranslateManager() = default;

    TranslateManager(const TranslateManager&) = delete;

    TranslateManager& operator=(const TranslateManager&) = delete;

#ifdef EASY_TRANSLATE_DUMP_TEXTID
    std::set<std::string> textIds_;
#endif // !EASY_TRANSLATE_DUMP_TEXTID
    std::string currentLanguage_;
    Languages languages_;
    Translations translations_;
};

// For convenience

inline TranslateManager& getTranslateManager()
{ return TranslateManager::getInstance(); }

/// @brief Get the `Translation text` of the given `Text ID` on current language.
/// @note If the given `Text ID` is not exist on the current language, return the `Text ID` itself.
inline const char* tr(const std::string& textId)
{ return getTranslateManager().translate(textId); }

/// @brief Update all `Translations file`s. (add pairs of the new `Text ID` and empty `Translation text`)
/// @return The number of updated files.
/// @note The new `Text ID` is from all `Text ID` that passed as #tr() function argument in programs.
/// @note This function can help you to easy get the all `Text ID` that need to translate.
/// @attention Make sure to call this function after you already call all #tr() function,
/// if not you will get incomplete `Text ID` list.
/// @attention This function is not effect when undefine the macro \ref EASY_TRANSLATE_DUMP_TEXTID.
inline size_t updateTranslationsFiles()
{ return getTranslateManager().updateTranslationsFiles(); }

/// @brief Set the `Languages`.
inline void setLanguages(const Languages& langs)
{ getTranslateManager().setLanguages(langs); }

/// @brief Set the `Languages` that from a json file.
inline void setLanguages(const std::string& filename)
{ getTranslateManager().setLanguages(filename); }

/// @brief Set the current language by `Language ID`.
/// @return If success to change return true else return false.
inline bool setCurrentLanguage(const std::string& languageId)
{ return getTranslateManager().setCurrentLanguage(languageId); }

/// @brief Get the number of the `Language ID`.
inline size_t languageCount()
{ return getTranslateManager().languageCount(); }

/// @brief Get the number of the `Text ID` on current language.
inline size_t textCount()
{ return getTranslateManager().textCount(); }

/// @brief Check whether exists the given `Language ID`.
inline bool hasLanguage(const std::string& languageId)
{ return getTranslateManager().hasLanguage(languageId); }

/// @brief Check whether exists the given `Text ID`.
inline bool hasText(const std::string& textId)
{ return getTranslateManager().hasText(textId); }

inline const char* currentLanguage()
{ return getTranslateManager().currentLanguage(); }

inline const Languages& languages()
{ return getTranslateManager().languages(); }

inline const Translations& translations()
{ return getTranslateManager().translations(); }

} // namespace easytr

#endif // !EASY_TRANSLATE_HPP
