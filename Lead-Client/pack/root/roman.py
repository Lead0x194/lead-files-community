def IntegerToRoman(num):
    symbols = {
        1: 'I',
        4: 'IV',
        5: 'V',
        9: 'IX',
        10: 'X',
        40: 'XL',
        50: 'L',
        90: 'XC',
        100: 'C',
        400: 'CD',
        500: 'D',
        900: 'CM',
        1000: 'M'
    }

    roman_result = ""
    for value, numeral in sorted(symbols.items(), reverse=True):
        while num >= value:
            roman_result += numeral
            num -= value

    return roman_result


def GenerateRomanNumerals(length):
    roman_numerals = [IntegerToRoman(i + 1) for i in range(length)]
    return roman_numerals
