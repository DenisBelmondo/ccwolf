#include <fstream>
#include "cc_config.hpp"

using configmap_t = std::unordered_map<std::string, std::string>;

configmap_t config_map = configmap_t();

std::unordered_map<std::string, std::string> CcWolf::get_config()
{
    return config_map;
}

void CcWolf::read_config(std::string p_path)
{
    std::ifstream input(p_path);

    if (!input.is_open())
    {
        // could not open config file, create new?
        return;
    }

    size_t line = 1;
    char c = input.get();

    auto is_at_eof = [&]() -> bool
    {
        input.get();

        if (input.eof())
        {
            return true;
        }

        input.unget();
        return false;
    };

    auto is_legal_iden_char = [](char c) -> bool
    {
        return !std::isspace(c);
    };

    auto skip_whitespace = [&]()
    {
        while (std::isspace(c))
        {
            if (c == '\n')
            {
                line += 1;
            }

            input.get(c);
        }
    };

    auto skip_comment = [&]()
    {
        while (c != '\n')
        {
            input.get(c);
        }

        line += 1;
        input.get(c);
    };

    auto skip_comments = [&]()
    {
        while (c == '#' || c == ';')
        {
            skip_comment();
        }
    };

    auto parse_keyval = [&]() -> std::string
    {
        std::string tok = "";

        while (is_legal_iden_char(c))
        {
            tok += c;
            input.get(c);
        }

        return tok;
    };

    auto parse_eq = [&]() -> char
    {
        if (c == '=')
        {
            input.get(c);
            return true;
        }

        // expected '='
        return false;
    };

    while (!is_at_eof())
    {
        skip_whitespace();
        skip_comments();
        skip_whitespace();

        auto key = parse_keyval();
        skip_whitespace();
        if (!parse_eq())
        {
            printf("%s line %zu: expected '=', got '%c'\n", p_path.c_str(), line, c);
            goto Close;
        }
        skip_whitespace();
        auto val = parse_keyval();

        config_map.insert(std::pair<std::string, std::string>(key, val));
    }

Close:
    input.close();
}

void CcWolf::write_config(std::string p_path)
{

}
