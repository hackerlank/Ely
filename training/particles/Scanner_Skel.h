#ifndef \@$_H_INCLUDED_
#define \@$_H_INCLUDED_

$insert baseclass_h

class ParticleEffect;

$insert namespace-open

$insert classHead

$insert 8 lexFunctionDecl

		void setParticleEffect(ParticleEffect* particleEffect)
		{
			this->particleEffect = particleEffect;
		}

    private:
        int lex__();
        int executeAction__(size_t ruleNr);

        void print();
        void preCode();     // re-implement this function for code that must
                            // be exec'ed before the patternmatching starts

        void postCode(PostEnum__ type);
                            // re-implement this function for code that must
                            // be exec'ed after the rules's actions.
        ParticleEffect* particleEffect;
};

$insert scannerConstructors

$insert inlineLexFunction

inline void \@::preCode()
{
    // optionally replace by your own code
}

inline void \@::postCode(PostEnum__ type)
{
    // optionally replace by your own code
}

inline void \@::print()
{
    print__();
}

$insert namespace-close

#endif // \@_H_INCLUDED_
