# Effective AI Coding

## Table of Contents

1. [Part 1: Core Philosophy](#part-1-core-philosophy)
2. [Part 2: Building the AI Knowledge Base (Long-Term Context)](#part-2-building-the-ai-knowledge-base-long-term-context)
3. [Part 3: Execution Workflows – The 4 Techniques](#part-3-execution-workflows--the-4-techniques)
    - [Technique 1: Context Management (Surviving the "Dumb Zone")](#technique-1-context-management--surviving-the-dumb-zone)
    - [Technique 2: The "Grill Me" Skill (Pre-Computation Alignment)](#technique-2-the-grill-me-skill--pre-computation-alignment)
    - [Technique 3: Vertical Slicing – Tracer Bullets](#technique-3-vertical-slicing--tracer-bullets)
    - [Technique 4: Test-Driven Development (TDD)](#technique-4-test-driven-development-tdd)
4. [Closing: Workflow Summary](#closing-workflow-summary)

---

<a id="part-1-core-philosophy"></a>

## Part 1: Core Philosophy

### "Specs to Code" Is Not Enough
When developers ask AI to build a whole feature from a single prompt, the expectation is simple: Write requirements → AI builds it → Ship it.

**Reality:** AI agents often produce mediocre, tangled code without proper guidance because AI does not inherently understand a project's architecture, taste, or long-term codebase health.

### Knowledge Gap
Most AI workflows fail because project knowledge never accumulates:
`Task → Explain project context → Prompt AI → Get output → Repeat next week`

This does not scale. We must transition from memory-less operations to an approach where AI is treated as a continuously learning team member by building a **Knowledge Base**.

**Insight**: Context engineering is more valuable than prompt engineering. A mediocre prompt with excellent curated project context outperforms an excellent prompt with zero context.

### The Division of Labor
- **Humans own:** Planning, architecture, QA, context curation, and taste.
- **AI owns:** Implementation of well-scoped tasks.
- **Collaboration mechanism:** Tests, code reviews, and structured knowledge retrieval (Knowledge Base) keep the AI on track.

---

<a id="part-2-building-the-ai-knowledge-base-long-term-context"></a>

## Part 2: Building the AI Knowledge Base (Long-Term Context)

[to be filled during presentation]

---

<a id="part-3-execution-workflows--the-4-techniques"></a>

## Part 3: Execution Workflows – The 4 Techniques

<a id="technique-1-context-management--surviving-the-dumb-zone"></a>

### Technique 1: Context Management (Surviving the "Dumb Zone")

#### What Is the "Dumb Zone"?

LLM attention mechanisms suffer from **quadratic performance degradation** with context length. Feed an LLM massive context, and it will:
- Silently drop constraints
- Hallucinate variables and dependencies
- Lose track of the core objective
- Generate generic, wrong solutions

This happens *silently*, the AI will confidently output plausible-but-incorrect code.

#### Why It Matters

Every line of old code, terminal output, chat history, and tangential information adds "token pollution." The AI's focus deteriorates in proportion to context size.

#### Strategy: The "Smart Zone"

**Clear context entirely between tasks. Start fresh with an isolated system prompt and only essential information.**

#### Implementation Checklist

- [ ] Clear chat context entirely between major tasks
- [ ] Provide only the minimal, isolated file(s) needed
- [ ] Write precise, specific prompts (not vague requests)
- [ ] Include the exact expected output or constraint
- [ ] For debugging: provide only the broken file, not the whole codebase
- [ ] For new features: provide only the interface, not implementation details

**Key Insight** - Our job is filtering, not copying. Curate what the AI sees. The quality of AI output is proportional to the signal-to-noise ratio of the context we provide.

<a id="technique-2-the-grill-me-skill--pre-computation-alignment"></a>

### Technique 2: The "Grill Me" Skill (Pre-Computation Alignment)

#### The Problem: Vague Prompts → Generic Architecture

When we ask an AI agent to "build a feature," it will:
- Eagerly agree with our vague requirements
- Generate plausible-but-generic architecture
- Miss critical edge cases and constraints
- Require total rewrites when reality emerges

The root cause: **ambiguity about what "done" actually means**.

#### Why This Technique Works

The "Grill Me" prompt forces the AI to interrogate us relentlessly *before* writing any code. This pre-computation alignment:
- Maps out edge cases
- Resolves design tree branches
- Clarifies constraints
- Produces a clear mental model that both we and the AI share

#### The Workflow

Instead of asking: *"Create a database schema for a video production supply chain"*

Ask: *"Interview me relentlessly about the database schema for a video production supply chain. Walk down the design tree resolving dependencies. Ask one question at a time and provide recommendations."*

#### Example: Video Production Supply Chain

**AI's Interrogation Flow:**

```
AI: "How is the 'Publish Conversion Rate' calculated? Is this 
    a materialized view updated nightly, or calculated on-the-fly 
    from an event stream?"
We: "Materialized view, updated nightly."

AI: "That means write-heavy traffic during the materialization window. 
    Should we use a separate write-optimized table during nightly updates, 
    or accept temporary locking?"
We: "Separate write-optimized table."

AI: "To ensure BCNF, should target metrics be functionally dependent on 
    exact Video_ID, or grouped by Platform_ID?"
We: "Composite key: (Video_ID, Platform_ID)."

AI: "That creates a many-to-many relationship between Videos and Platforms. 
    Do we need to track historical Platform assignments, or is current state 
    sufficient?"
We: "Historical tracking required for analytics."
```

#### Output: The Design Concept

The Q&A history itself becomes our immutable "Design Concept". We don't need a formal PRD document, the conversation acts as the requirements. Use this design concept when prompting the actual coding agent.

#### When to Use "Grill Me"

- Before building major features or systems
- Before architectural decisions
- When requirements are fuzzy
- When integrating multiple systems
- Not needed for trivial, isolated tasks

<a id="technique-3-vertical-slicing--tracer-bullets"></a>

### Technique 3: Vertical Slicing – Tracer Bullets

#### The Problem: Horizontal Implementation Fails

AI naturally codes "horizontally" (layer by layer):
1. Build entire database schema
2. Build entire API layer
3. Build entire UI layer

This approach **delays integration testing** and almost guarantees failure in complex systems. By the time layers connect, incompatibilities are discovered too late.

#### Why Vertical Slicing Works

A **tracer bullet** is a hyper-narrow slice of functionality that cuts through **every layer** of the stack immediately. It:
- Proves the end-to-end path works
- Reveals format/size mismatches early
- Is demoable/testable on its own
- Builds momentum (visible progress)

#### Example: Unstructured PII Detection System

Consider a pipeline that processes PDFs, images, DOCX files, and spreadsheets:

```
Document
    ↓
Text Extraction
    ↓
PII Detection
    ↓
Masking
    ↓
Redacted Document
```

**Horizontal Slicing (Hides Integration Risks)**

| Phase | Work | Status |
|-------|------|--------|
| 1 | Build extraction support for PDF, Image, DOCX, XLSX | Complete |
| 2 | Integrate pluggable PII detectors | Complete |
| 3 | Build masking/redaction engine | Problem emerges |
| 4 | Add batch processing | Blocked |

**The hidden risk**: To redact PII, we need to know *where* the detected text came from in the original document. PDF and OCR libraries may provide page numbers and bounding boxes, but some DOCX extraction libraries only return plain text without location metadata.

Result: The system can successfully detect PII, but cannot reliably mask it. The issue isn't in the masking layer, it's an extraction-layer design decision discovered too late. Fixing it may require redesigning extraction contracts or replacing extraction libraries entirely.

**Vertical Slicing (Discovers Architectural Requirements Early)**

Instead of building all extraction formats first, start with a single complete path:

- **Slice 1**: Extract PDF → Detect PII → Mask PII (end-to-end with metadata preservation)
  - *Key insight discovered immediately*: Extraction must preserve location metadata for masking to work
- **Slice 2**: Add DOCX support (validates extraction contract)
- **Slice 3**: Add additional PII detector implementations
- **Slice 4**: Add image support
- **Slice 5**: Add batch processing

Each slice builds on proven foundations. The extraction-masking integration is validated on day 1, not day 90.

#### Implementation Pattern

For any feature:

1. **Identify layers**: UI → API → Business Logic → Data
2. **Pick the simplest path**: Hardcode data, minimal logic
3. **Build one complete slice**: Walk through every layer end-to-end
4. **Verify and test**: Confirm integration works
5. **Expand incrementally**: Add complexity one slice at a time

#### Why AI Loves Vertical Slices

- Small scope = stays in "Smart Zone" (low token pollution)
- Concrete, testable milestones = clear acceptance criteria
- Failures are isolated to one layer = easy debugging
- Each success builds momentum

<a id="technique-4-test-driven-development-tdd"></a>

### Technique 4: Test-Driven Development (TDD)

#### The Problem: AI Optimizes for Passing, Not Necessarily Correctness

Suppose we tell an AI:

> Build a PII masking service.

The request sounds simple, but the important behavior is undefined:

- What counts as PII?
- Should emails be masked?
- Should phone numbers be masked?
- What should the replacement format look like?
- What happens if no PII exists?
- How should PDFs, CSVs, and images be handled?

Without explicit constraints, the AI fills in the blanks. The code may look reasonable while violating the real requirement.

---

#### What TDD Actually Does

> TDD forces us to define expected behavior before implementation exists.

For AI coding, tests are not just quality checks. They are an executable specification. Instead of letting the model infer behavior from a vague prompt, we encode the behavior first and make the implementation satisfy it.

---

##### Phase 1: Red (Define the Behavior)

Before any implementation exists, define the expected outcome.

```python
def test_email_is_masked():
    text = "Contact me at john@example.com"

    result = mask_pii(text)

    assert result == "Contact me at [EMAIL]"
```

This single test establishes four requirements:

- Emails are considered PII
- Emails must be masked
- The replacement format is `[EMAIL]`
- The surrounding text must remain unchanged

At this point the function does not exist, so the test fails. That failure is useful: it proves the requirement is now concrete.

---

##### Phase 2: Green (Make the Test Pass)

Now instruct the AI to implement the smallest amount of code needed to satisfy the test.

Example:

```python
import re

def mask_pii(text):
    return re.sub(
        r'\S+@\S+',
        '[EMAIL]',
        text
    )
```

The implementation is no longer based on the AI's assumptions. It is constrained by the behavior defined in the test.

---

##### Phase 3: Refactor (Improve Without Changing Behavior)

Once the behavior is locked, we can improve the implementation without changing the contract:

- improve performance
- use a dedicated PII library
- switch to a machine learning model
- add caching

Example:

```python
def mask_pii(text):
    entities = pii_detector.detect(text)

    for entity in entities:
        text = text.replace(entity.value, entity.label)

    return text
```

As long as the original tests still pass, the external behavior remains intact.

---

#### What Tests Should We Write?

Write tests for behavior, not implementation. Good TDD coverage usually includes three categories:

**Business rules**: what must happen.

```python
def test_email_is_masked():
    ...

def test_phone_number_is_masked():
    ...
```

**Edge cases**: what commonly breaks.

```python
def test_empty_string():
    ...

def test_no_pii_present():
    ...

def test_multiple_emails():
    ...
```

**Invariants**: what must always remain true.

```python
def test_non_pii_text_is_preserved():
    ...

def test_document_line_count_is_unchanged():
    ...
```

Invariants are often more valuable than exact-output tests because they capture fundamental system behavior.

---

#### Preventing Overfitting to Tests

The AI can overfit to weak tests. If the only test is:

```python
def test_email_masking():
    assert mask_pii(
        "Contact me at john@example.com"
    ) == "Contact me at [EMAIL]"
```

The AI could satisfy it with a hardcoded implementation:

```python
def mask_pii(text):
    return "Contact me at [EMAIL]"
```

The test passes, but the implementation is wrong. The constraint was too weak. Prevent this with broader and stronger tests:

**Add more examples**:

```python
def test_email_1():
    ...

def test_email_2():
    ...

def test_email_3():
    ...
```

**Use different input shapes**:

```python
def test_various_email_formats():
    ...

def test_multiple_emails():
    ...

def test_email_inside_sentence():
    ...
```

**Test properties instead of only examples**:

```python
assert detected_email not in result
assert len(result) > 0
```

These tests validate general behavior instead of one memorized output.

---

#### At What Level Should TDD Tests Exist?

For AI-generated code, higher-level tests are often more valuable than isolated unit tests.

**Unit tests** verify small pieces of logic:

```python
test_email_detection()
test_phone_detection()
```

**Integration tests** verify components working together:

```python
test_pdf_extraction_then_masking()
```

**End-to-end tests** verify behavior from the user's perspective:

```python
input.pdf
    ↓
process()
    ↓
output.pdf
```

Example:

```python
def test_pdf_contains_no_pii_after_processing():
    ...
```

End-to-end tests are often the hardest for AI to game because they validate complete workflows rather than isolated functions.

---

#### What Makes a Good TDD Test?

A good test answers:

> If this test passes, what real-world requirement have I proven?

Good:

```python
assert "john@example.com" not in result
```

Bad:

```python
assert result is not None
```

The first proves that sensitive data was removed. The second proves almost nothing.

---

#### The Biggest Benefit of TDD for AI Coding

TDD does not guarantee correctness. A weak test suite still produces weak software.

What TDD gives us is a mechanism for converting vague requirements into concrete, executable constraints:

```text
Requirement
    ↓
Tests encode expected behavior
    ↓
AI writes code to satisfy behavior
    ↓
Refactoring remains safe
```

That contract is especially valuable with AI because every missing requirement is an opportunity for the model to make assumptions. TDD removes those assumptions by making expected behavior explicit before code exists.

---

<a id="closing-workflow-summary"></a>

## Closing: Workflow Summary

Effective AI coding is not a single prompt. It is a workflow:

1. **Knowledge base**: Preserve project context so the AI is not starting from zero every time.
2. **Context control**: Keep each task inside the "Smart Zone" with only the files, constraints, and outputs that matter.
3. **Grill Me**: Resolve ambiguity before implementation by forcing design questions early.
4. **Vertical slice**: Build the smallest end-to-end path first so integration risks appear immediately.
5. **TDD**: Convert expected behavior into executable constraints before asking the AI to implement.

The pattern is simple: give the AI durable knowledge, constrain its working context, clarify the design, prove the integration path, and lock behavior with tests.
