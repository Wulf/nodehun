declare module 'nodehun' {
  export class Nodehun {
    constructor(affix: Buffer, dictionary: Buffer);
    
    spell(word: string): Promise<boolean>;
    spellSync(word: string): boolean;
      
    suggest(word: string): Promise<string[] | null>;
    suggestSync(word: string): string[] | null;
      
    analyze(word: string): Promise<string[]>;
    analyzeSync(word: string): string[];
    
    stem(word: string): Promise<string[]>;
    stemSync(word: string): string[];
    
    generate(word: string, example: string): Promise<string[]>;
    generateSync(word: string, example: string): string[];
  
    addDictionary(dictionary: Buffer): Promise<void>;
    addDictionarySync(dictionary: Buffer): void;
    
    add(word: string): Promise<void>;
    addSync(word: string): void;
    
    addWithAffix(word: string, example: string): Promise<void>;
    addWithAffixSync(word: string, example: string): void;
    
    remove(word: string): Promise<void>;
    removeSync(word: string): void;
    
    getDictionaryEncoding(): string | undefined;
    getWordCharacters(): string | undefined;
    getWordCharactersUTF16(): string | undefined;
    getVersion(): string | undefined;
  }  
}
