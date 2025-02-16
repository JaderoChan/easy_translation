// The "easy translation" library, in c++.
//
// Webs: https://github.com/JaderoChan/easy_translation
// You can contact me by email: c_dl_cn@outlook.com

// GPL-3.0 License
//
// Copyright (c) 2024 頔珞JaderoChan

#ifndef EASY_TRANSLATION_HPP
#define EASY_TRANSLATION_HPP

#include <cstddef>              // size_t
#include <string>               // string
#include <vector>               // vector
#include <set>                  // set
#include <map>                  // map
#include <fstream>              // ifstream
#include <stdexcept>            // runtime_error

#include <nlohmann/json.hpp>    // json

/// @attention Don't use the json with comments.

/// @brief Define this macro to enable release mode, for better performance.
/// @note If you not define this macro, the #TranslaeManager::translate() function will store
/// all the translation text IDs in memory used for possible update the translation files.
// #define EASY_TRANSLATION_RELEASE

// - Translation function
//   - Usage: EASYTR("Translation text ID")
//   - Returns the translated text.
#define EASYTR(x) easytr::tr(x)

// - Language list
//   - Language ID : Translation list file
//   - ...
//
// - Translation list
//   - Translation text ID : Translation text
//   -...

// The following is a sample directory structure and content structure for language files:
//
// - languages.json (Language list)
//   - en_US (Language ID) : en_US.json (Translation list file)
//   - zh_CN (Language ID) : zh_CN.json (Translation list file)
//   - ...
//   - ja_JP (Language ID) : ja_JP.json (Translation list file)
//   - fr_FR (Language ID) : fr_FR.json (Translation list file)
//
// - en_US.json (Translation list)
//   - App.Title  (Translation text ID) : Easy Translation (Translation text)
//   - App.Author (Translation text ID) : JaderoChan       (Translation text)
//   - ...
//
// - zh_CN.json
//   - App.Title  (Translation text ID) : 轻松翻译 (Translation text)
//   - App.Author (Translation text ID) : 頔珞     (Translation text)
//   - ...

namespace easytr
{

class LanguageList
{
    friend class TranslateManager;
public:
    LanguageList() = default;

    LanguageList(const std::vector<std::pair<std::string, std::string>>& list)
    {
        for (const auto& var : list)
            languages_.insert({ var.first, var.second });
    }

    LanguageList(const std::map<std::string, std::string>& list) : languages_(list) {}

    /// @brief Load the language list from a json string.
    /// @param json The json string of the language list.
    /// @attention If the json is invalid, the language list will be empty.
    static LanguageList fromJson(const std::string& json)
    {
        using Json = nlohmann::json;

        try
        {
            Json j = Json::parse(json);
            std::map<std::string, std::string> list;
            for (const auto& var : j.items())
                list.insert({ var.key(), var.value() });

            return LanguageList(list);
        }
        catch (const std::exception& e)
        {
            return LanguageList();
        }

    }

    /// @brief Load the language list from a json file.
    /// @param filename The file name of the language list json file.
    /// @attention If the json is invalid, the language list will be empty.
    static LanguageList fromJsonFile(const std::string& filename)
    {
        using Json = nlohmann::json;

        std::ifstream ifs(filename);
        if (!ifs.is_open())
            throw std::runtime_error("Can't open the file: " + filename);

        try
        {
            std::map<std::string, std::string> list;
            Json j = Json::parse(ifs);
            ifs.close();
            for (const auto& var : j.items())
                list.insert({ var.key(), var.value() });

            return LanguageList(list);
        }
        catch (const std::exception& e)
        {
            ifs.close();
            return LanguageList();
        }
    }

    /// @brief Get the translation list file of the given language.
    /// @param languageId The language ID.
    /// @return The file name of the translation list.
    std::string translationListFile(const std::string& languageId) const { return languages_.at(languageId); }

    /// @brief Get the number of languages in the language list.
    size_t count() const { return languages_.size(); }

    /// @brief Check if the language list is empty.
    bool isEmpty() const { return count() == 0; }

    /// @brief Check if the given language id exists in the language list.
    bool has(const std::string& languageId) const { return languages_.find(languageId) != languages_.end(); }

    /// @brief Get all language IDs in the language list.
    std::vector<std::string> languageIds() const
    {
        std::vector<std::string> ids;
        for (const auto& var : languages_)
            ids.push_back(var.first);

        return ids;
    }

    /// @brief Write the language list to a json file.
    /// @param filename The file name of the language list.
    void write(const std::string& filename = "languages.json") const
    {
        using Json = nlohmann::json;

        Json j;
        for (const auto& var : languages_)
            j[var.first] = var.second;

        std::ofstream ofs(filename);
        if (!ofs.is_open())
            throw std::runtime_error("Can't open the file: " + filename);

        ofs << j.dump(4);
        ofs.close();
    }

    /// @brief Add a map of language ID and translation list file.
    /// @param languageId The language ID.
    /// @param translationListFile The file name of the translation list.
    /// @note If the given language id already exists, do nothing.
    void add(const std::string& languageId, const std::string& translationListFile)
    {
        if (has(languageId))
            return;

        languages_.insert({ languageId, translationListFile });
    }

    /// @brief Remove a language.
    /// @param languageId The language ID.
    void remove(const std::string& languageId) { languages_.erase(languageId); }

    /// @brief Remove all languages.
    void clear() { languages_.clear(); }

private:
    // {Language ID : Translation list file}
    std::map<std::string, std::string> languages_;
};

class TranslationList
{
    friend class TranslateManager;
public:
    TranslationList() = default;

    TranslationList(const std::vector<std::pair<std::string, std::string>>& list)
    {
        for (const auto& var : list)
            translations_.insert({ var.first, var.second });
    }

    TranslationList(const std::map<std::string, std::string>& list) : translations_(list) {}

    /// @brief Load the translation list from a json string.
    /// @param json The json string of the translation list.
    /// @attention If the json is invalid, the translation list will be empty.
    static TranslationList fromJson(const std::string& json)
    {
        using Json = nlohmann::json;

        try
        {
            std::map<std::string, std::string> list;
            Json j = Json::parse(json);
            for (const auto& var : j.items())
                list.insert({ var.key(), var.value() });

            return TranslationList(list);
        }
        catch (const std::exception& e)
        {
            return TranslationList();
        }
    }

    /// @brief Load the translation list from a json file.
    /// @param filename The file name of the translation list json file.
    /// @attention If the json is invalid, the translation list will be empty.
    static TranslationList fromJsonFile(const std::string& filename)
    {
        using Json = nlohmann::json;

        std::ifstream ifs(filename);
        if (!ifs.is_open())
            throw std::runtime_error("Can't open the file: " + filename);

        try
        {
            std::map<std::string, std::string> list;
            Json j = Json::parse(ifs);
            ifs.close();
            for (const auto& var : j.items())
                list.insert({ var.key(), var.value() });

            return TranslationList(list);
        }
        catch (const std::exception& e)
        {
            ifs.close();
            return TranslationList();
        }
    }

    /// @brief Get the translation text of the given text ID.
    /// @param textId The text ID.
    /// @return The translated text.
    /// @note If the given text ID doesn't exist, return the text ID itself.
    const char* translationText(const std::string& textId) const
    {
        if (!has(textId))
            return textId.c_str();
        return translations_.at(textId).c_str();
    }

    /// @brief Get the number of translations in the translation list.
    size_t count() const { return translations_.size(); }

    /// @brief Check if the translation list is empty.
    bool isEmpty() const { return count() == 0; }

    /// @brief Check if the given text ID exists in the translation list.
    bool has(const std::string& textId) const { return translations_.find(textId) != translations_.end(); }

    /// @brief Write the translation list to a json file.
    /// @param filename The file name of the translation list.
    void write(const std::string& filename) const
    {
        using Json = nlohmann::json;

        Json j;
        for (const auto& var : translations_)
            j[var.first] = var.second;

        std::ofstream ofs(filename);
        if (!ofs.is_open())
            throw std::runtime_error("Can't open the file: " + filename);

        ofs << j.dump(4);
        ofs.close();
    }

    /// @brief Add a map of text ID and translation text.
    /// @param textId The text ID.
    /// @param text The translation text.
    /// @note If the given text ID already exists, do nothing.
    void add(const std::string& textId, const std::string& text)
    {
        if (has(textId))
            return;

        translations_.insert({ textId, text });
    }

    /// @brief Remove a translation.
    /// @param textId The text ID.
    void remove(const std::string& textId) { translations_.erase(textId); }

    /// @brief Remove all translations.
    void clear() { translations_.clear(); }

private:
    // {Translation text ID : Translation text}
    std::map<std::string, std::string> translations_;
};

class TranslateManager
{
public:
    static TranslateManager& getInstance()
    {
        static TranslateManager instance;
        return instance;
    }

    /// @brief Get the translation text of the given text ID.
    /// @param textId The text ID.
    /// @return The translated text.
    /// @note If the given text ID doesn't exist in the current language, return the text ID itself.
#ifdef EASY_TRANSLATION_RELEASE
    const char* translate(const std::string& textId) const
    {
        return translations_.translationText(textId);
    }
#else
    /// @brief Get the translation text of the given text ID.
    /// @param textId The text ID.
    /// @return The translated text.
    /// @note If the given text ID doesn't exist in the current language, return the text ID itself.
    const char* translate(const std::string& textId)
    {
        textIds_.insert(textId);
        return translations_.translationText(textId);
    }
#endif // EASY_TRANSLATION_RELEASE

    /// @brief Update the translation files by add new text IDs and a empty translation.
    /// @note The new text IDs is from all text ID that passed as #translate() function argument in programs.
    /// @note This function can help you to easy get the all text ID that need to translate.
    /// @attention Make sure to call this function after you already call all #translate() function,
    /// if not you will get incomplete text IDs list.
    /// @attention This function is not effect when define the macro #EASY_TRANSLATION_RELEASE.
    void updateTranslationFiles() const
    {
    #ifdef EASY_TRANSLATION_RELEASE
        return;
    #else
        using Json = nlohmann::json;

        for (const auto& languageId : languages_.languageIds())
        {
            std::string filename = languages_.translationListFile(languageId);
            std::ifstream ifs(filename);
            Json j;
            if (!ifs.is_open())
            {
                for (const auto& textId : textIds_)
                    j[textId] = "";
            }
            else
            {
                try
                {
                    j = Json::parse(ifs);
                    std::map<std::string, std::string> map; // For sorting.
                    for (const auto& textId : textIds_)
                        j.contains(textId) ? map.insert({ textId, j[textId] }) : map.insert({ textId, "" });

                    j.clear();
                    for (const auto& var : map)
                        j[var.first] = var.second;
                }
                catch (const std::exception& e)
                {
                    for (const auto& textId : textIds_)
                        j[textId] = "";
                }

                ifs.close();
            }

            std::ofstream ofs(filename);
            if (!ofs.is_open())
                throw std::runtime_error("Can't open the file: " + filename);

            ofs << j.dump(4);
            ofs.close();
        }
    #endif // EASY_TRANSLATION_RELEASE
    }

    /// @brief Set the language list.
    /// @param list The language list.
    void setLanguageList(const LanguageList& languageList) { languages_ = languageList; }

    /// @brief Set the language list from a json file.
    /// @param filename The file name of the language list.
    void setLanguageList(const std::string& filename) { languages_ = LanguageList::fromJsonFile(filename); }

    /// @brief Set the current language.
    /// @param languageId The language ID.
    void setLanguage(const std::string& languageId)
    {
        if (!hasLanguage(languageId))
            throw std::runtime_error("Language ID not found: " + languageId);

    #ifndef EASY_TRANSLATION_RELEASE
        bool isFirst = currentLanguageId_.empty();
    #endif // !EASY_TRANSLATION_RELEASE
        currentLanguageId_ = languageId;
        translations_ = TranslationList::fromJsonFile(languages_.translationListFile(languageId));

    #ifndef EASY_TRANSLATION_RELEASE
        if (isFirst)
        {
            for (const auto& var : translations_.translations_)
                textIds_.insert(var.first);
        }
    #endif // !EASY_TRANSLATION_RELEASE
    }

    size_t languageCount() const { return languages_.count(); }

    bool hasLanguage(const std::string& languageId) const { return languages_.has(languageId); }

    bool hasTranslation(const std::string& textId) const { return translations_.has(textId); }

    const char* currentLanguageId() const { return currentLanguageId_.c_str(); }

    std::vector<std::string> allLanguageId() const { return languages_.languageIds(); }

    const LanguageList& languageList() const { return languages_; }

    const TranslationList& translationList() const { return translations_; }

private:
    TranslateManager() = default;

    ~TranslateManager() = default;

    TranslateManager(const TranslateManager&) = delete;

    TranslateManager& operator=(const TranslateManager&) = delete;

#ifndef EASY_TRANSLATION_RELEASE
    std::set<std::string> textIds_;
#endif // !EASY_TRANSLATION_RELEASE
    std::string currentLanguageId_;
    LanguageList languages_;
    TranslationList translations_;
};

// For convenience.

inline TranslateManager& getTranslateManager() { return TranslateManager::getInstance(); }

/// @brief Get the translation text of the given text ID.
/// @param textId The text ID.
/// @return The translated text.
/// @note If the given text ID doesn't exist in the current language, return the text ID itself.
inline const char* tr(const std::string& textId) { return getTranslateManager().translate(textId); }

/// @brief Update the translation files by add new text IDs and a empty translation.
/// @note The new text IDs is from all text ID that passed as #translate() function argument in programs.
/// @note This function can help you to easy get the all text ID that need to translate.
/// @attention Make sure to call this function after you already call all #translate() function,
/// if not you will get incomplete text IDs list.
/// @attention This function is not effect when define the macro #EASY_TRANSLATION_RELEASE.
inline void updateTranslationFiles() { getTranslateManager().updateTranslationFiles(); }

/// @brief Set the language list.
/// @param list The language list.
inline void setLanguageList(const LanguageList& list) { getTranslateManager().setLanguageList(list); }

/// @brief Set the language list from a json file.
/// @param filename The file name of the language list.
inline void setLanguageList(const std::string& filename) { getTranslateManager().setLanguageList(filename); }

/// @brief Set the current language.
/// @param languageId The language ID.
inline void setLanguage(const std::string& languageId) { getTranslateManager().setLanguage(languageId); }

inline size_t languageCount() { return getTranslateManager().languageCount(); }

inline bool hasLanguage(const std::string& languageId) { return getTranslateManager().hasLanguage(languageId); }

inline bool hasTranslation(const std::string& textId) { return getTranslateManager().hasTranslation(textId); }

inline const char* currentLanguageId() { return getTranslateManager().currentLanguageId(); }

inline std::vector<std::string> allLanguageId() { return getTranslateManager().allLanguageId(); }

inline const LanguageList& languageList() { return getTranslateManager().languageList(); }

inline const TranslationList& translationList() { return getTranslateManager().translationList(); }

} // namespace easytr

#endif // !EASY_TRANSLATION_HPP
