1. Loading the folders with trait and fill GenerationInput class with data.
2. Calculate equal generation chances the first generation folder.

    -> function (pass not by reference main array with folders)
    2.1. Load exceptions for generated trait .
    2.2. Generate randomly an asset in a folder.
    2.3. Apply exceptions if any for the generated asset.
    2.4. Reapply calculation of generation chances if any asset is being excluded or exception is applied. 
    -- That repeats





1. Backgound 
2. Body
3. Head
4. Arms
5. Legs
6. Extras 


Models - all classes that just contain some data. example: 
class TraitFolder
{
    TraitFolder()
        : _generationChance(0.0)
    {
    }

    virtual ~TraitFolder() = default;

private:
    std::string _path;
    std::vector<Trait> _traits;
    double _generationChance;
};



workflow -> all logic 
corekit - core kit functions/classes etc.
include -> external header/cpp file dependencies / libs /*that should be a different section in makefile that is compilable*/
