# Effective AI Coding & The AI Second Brain: A Comprehensive Strategy

**Presentation Report | Duration: 30 minutes**

This document combines the core philosophies of **Effective AI Coding** and building an **AI Second Brain**. Together, they form a robust framework for scaling AI-assisted software engineering without falling into the traps of messy code and degraded AI context windows.

---

## Table of Contents

1. [Part 1: Core Philosophy](#part-1-core-philosophy)
2. [Part 2: Building the AI Second Brain (Long-Term Context)](#part-2-building-the-ai-second-brain-long-term-context)
3. [Part 3: Execution Workflows – The 5 Techniques](#part-3-execution-workflows--the-5-techniques)
    - [Technique 1: Context Management (Surviving the "Dumb Zone")](#technique-1-context-management--surviving-the-dumb-zone)
    - [Technique 2: The "Grill Me" Skill (Pre-Computation Alignment)](#technique-2-the-grill-me-skill--pre-computation-alignment)
    - [Technique 3: Vertical Slicing – Tracer Bullets](#technique-3-vertical-slicing--tracer-bullets)
    - [Technique 4: Test-Driven Development (TDD) – The Ultimate Constraint](#technique-4-test-driven-development-tdd--the-ultimate-constraint)
    - [Technique 5: Deep vs. Shallow Modules](#technique-5-deep-vs-shallow-modules)
4. [Part 4: Key Takeaways & Operational Habits](#part-4-key-takeaways--operational-habits)

---

## Part 1: Core Philosophy

### "Specs to Code" Is Not Enough
When developers ask AI to build a whole feature from a single prompt, the expectation is simple: Write requirements → AI builds it → Ship it.

**Reality:** This approach fails repeatedly. AI agents produce mediocre, tangled code without proper guidance because AI does not inherently understand a project's architecture, taste, or long-term codebase health. **Good software engineering fundamentals don't become obsolete with AI—they become mandatory.**

### Treat AI Like an Engineer, Not a Search Engine
Most AI workflows fail because project knowledge never accumulates:
`Task → Explain project context → Prompt AI → Get output → Repeat next week`

This does not scale. We must transition from memory-less operations to an approach where AI is treated as a continuously learning team member by building a **Second Brain**.

**The Key Insight**: Context engineering is more valuable than prompt engineering. A mediocre prompt with excellent curated project context outperforms an excellent prompt with zero context.

### The Division of Labor
- **Humans own:** Planning, architecture, QA, context curation, and taste.
- **AI owns:** Implementation of well-scoped tasks.
- **Collaboration mechanism:** Tests, code reviews, and structured knowledge retrieval (Second Brain) keep the AI on track.

---

## Part 2: Building the AI Second Brain (Long-Term Context)

Feeding an LLM your entire codebase degrades its performance quadratically. To combat this, you need a **Second Brain**—a structured knowledge base that captures durable context.

### Principles of the Second Brain
1. **Store Knowledge, Not Conversations**: Keep System architectures, ADRs, troubleshooting guides, and domain knowledge. Do not store full chat histories.
2. **Retrieve, Don't Inject**: Do not load 50 markdown files into every prompt. Load an index, find what's relevant, and retrieve only what's needed.

### The Architecture of Knowledge
Keep your Second Brain separated from the main codebase logic, organized in layers:

```text
project/
│
├── .clinerules / CLAUDE.md / AGENTS.md (Bootstrap)
│
├── knowledge/
│   ├── INDEX.md
│   ├── CURRENT_WORK.md
│   ├── architecture/
│   ├── decisions/
│   ├── troubleshooting/
```

#### Layer 1: Agent Bootstrap
A small file instructing the agent how to navigate your documentation. It is not the second brain itself, but the map.
```md
Read:
1. knowledge/INDEX.md
2. knowledge/CURRENT_WORK.md
Use INDEX.md as the retrieval map. Load additional files only when relevant.
```

#### Layer 2: Active Context
**INDEX.md**: The most important file. The retrieval map.
```md
# Knowledge Index

## Architecture
Auth System (File: architecture/auth.md, Tags: oauth, jwt)

## Decisions
ADR-001 PostgreSQL Selection
```

**CURRENT_WORK.md**: The bridge between long-term memory and today.
```md
# Current Sprint
Goals: Billing migration
Open Issues: Retry strategy undecided
Recent Decisions: Cache TTL increased to 15 minutes
```

#### Layer 3: Long-Term Memory
- **Architecture Documentation**: High-level system maps.
- **Architecture Decision Records (ADRs)**: Critical for context. Example:
  ```md
  # ADR-001
  Decision: Use PostgreSQL instead of MongoDB.
  Reason: Billing workflows require transactional guarantees.
  ```
- **Troubleshooting Knowledge**: Capture operational knowledge explicitly. Example: 
  *Symptoms: Increased latency. Root Cause: Connection pool exhaustion. Resolution: Increase max_connections to 100.*

---

## Part 3: Execution Workflows – The 5 Techniques

### Technique 1: Context Management (Surviving the "Dumb Zone")

#### What Is the "Dumb Zone"?

LLM attention mechanisms suffer from **quadratic performance degradation** with context length. Feed an LLM massive context, and it will:
- Silently drop constraints
- Hallucinate variables and dependencies
- Lose track of the core objective
- Generate generic, wrong solutions

This happens *silently*—the AI will confidently output plausible-but-incorrect code.

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

#### Example: Python Topological Sort Debugging

**❌ Failing Strategy (Dumb Zone)**

```
Context:
- Entire 50-file project codebase
- 3 previous chat messages about failed attempts
- Stack traces from failed test runs
- Multiple versions of the algorithm in chat history
- Vague prompt: "Fix the topological sort function"

Result:
- AI gets confused by old iterations visible in chat
- Invents helper functions not in the original code
- Generates generic DFS without cycle detection
- Output ignores the specific constraint: "must handle cyclic dependency detection"
```

**What AI sees (confusing mix of old and new):**

```python
# Old attempt (from chat history):
def topo_sort_v1(graph):
    visited = set()
    def dfs(node):
        visited.add(node)
        for neighbor in graph[node]:
            dfs(neighbor)
    return list(visited)

# New attempt (also in chat history):
def topo_sort_v2(graph):
    # ... different implementation

# Actual current code (what you want fixed):
def topological_sort(graph):
    visited = set()
    stack = []
    def dfs(node):
        visited.add(node)
        for neighbor in graph[node]:
            dfs(neighbor)
        stack.append(node)
    
    for node in graph:
        if node not in visited:
            dfs(node)
    return stack[::-1]

# Test that's failing:
def test_cyclic_graph():
    cyclic_graph = {
        "A": ["B"],
        "B": ["C"],
        "C": ["A"]  # Creates cycle
    }
    result = topological_sort(cyclic_graph)
    assert result is None or result == []  # Should detect cycle
```

**AI's confused output:**
- Can't tell which version is current
- Doesn't know cycle detection is required
- Outputs generic DFS that will infinite loop on cycles

---

**✅ Winning Strategy (Smart Zone)**

**Context:**
- ONLY the `topological_sort.py` file
- ONLY the failing `test_topo.py`
- Precise prompt (no ambiguity)

**What you provide:**

```python
# topological_sort.py - ONLY THIS FILE

def topological_sort(graph):
    """
    Topological sort of a directed acyclic graph (DAG).
    
    Args:
        graph: dict of {node: [neighbors]}
    
    Returns:
        List of nodes in topological order, or None if cycle detected
    """
    visited = set()
    stack = []
    
    def dfs(node):
        visited.add(node)
        for neighbor in graph[node]:
            dfs(neighbor)
        stack.append(node)
    
    for node in graph:
        if node not in visited:
            dfs(node)
    
    return stack[::-1]

# test_topo.py - ONLY THIS TEST
def test_cyclic_graph():
    cyclic_graph = {
        "A": ["B"],
        "B": ["C"],
        "C": ["A"]  # Creates cycle
    }
    result = topological_sort(cyclic_graph)
    assert result is None or result == []
```

**Precise prompt:**
> "This topological_sort() infinite loops on cyclic graphs. Add cycle detection using a visiting state array (0=unvisited, 1=visiting, 2=visited). Return None if a cycle is detected. Make the test pass."

**AI's correct output:**

```python
def topological_sort(graph):
    """Topological sort with cycle detection."""
    visited = {node: 0 for node in graph}  # 0=unvisited, 1=visiting, 2=visited
    stack = []
    
    def dfs(node):
        if visited[node] == 1:  # Currently visiting = cycle detected
            raise ValueError("Cycle detected in graph")
        if visited[node] == 2:  # Already processed
            return
        
        visited[node] = 1  # Mark as visiting
        for neighbor in graph[node]:
            dfs(neighbor)
        visited[node] = 2  # Mark as visited
        stack.append(node)
    
    try:
        for node in graph:
            if visited[node] == 0:
                dfs(node)
        return stack[::-1]
    except ValueError:
        return None  # Cycle detected

# Test passes ✅
```

**Result:**
- Zero token pollution
- AI correctly identifies visited state tracking requirement
- Perfect cycle detection implementation
- Test passes immediately

#### Key Insight

**Your job is filtering, not copying.** Curate what the AI sees. The quality of AI output is proportional to the signal-to-noise ratio of the context you provide.

### Technique 2: The "Grill Me" Skill (Pre-Computation Alignment)

#### The Problem: Vague Prompts → Generic Architecture

When you ask an AI agent to "build a feature," it will:
- Eagerly agree with your vague requirements
- Generate plausible-but-generic architecture
- Miss critical edge cases and constraints
- Require total rewrites when reality emerges

The root cause: **ambiguity about what "done" actually means**.

#### Why This Technique Works

The "Grill Me" prompt forces the AI to interrogate you relentlessly *before* writing any code. This pre-computation alignment:
- Maps out edge cases
- Resolves design tree branches
- Clarifies constraints
- Produces a clear mental model both you and the AI share

#### The Workflow

Instead of asking: *"Create a database schema for a video production supply chain"*

Ask: *"Interview me relentlessly about the database schema for a video production supply chain. Walk down the design tree resolving dependencies. Ask one question at a time and provide recommendations."*

#### Example: Video Production Supply Chain

**AI's Interrogation Flow:**

```
AI: "How is the 'Publish Conversion Rate' calculated? Is this 
     a materialized view updated nightly, or calculated on-the-fly 
     from an event stream?"
You: "Materialized view, updated nightly."

AI: "That means write-heavy traffic during the materialization window. 
     Should we use a separate write-optimized table during nightly updates, 
     or accept temporary locking?"
You: "Separate write-optimized table."

AI: "To ensure BCNF, should target metrics be functionally dependent on 
     exact Video_ID, or grouped by Platform_ID?"
You: "Composite key: (Video_ID, Platform_ID)."

AI: "That creates a many-to-many relationship between Videos and Platforms. 
     Do we need to track historical Platform assignments, or is current state 
     sufficient?"
You: "Historical tracking required for analytics."
```

#### Output: The Design Concept (Not a PRD)

The Q&A history itself becomes your **immutable "Design Concept"**. You don't need a formal PRD document—the conversation IS the requirements.

Use this design concept when prompting the actual coding agent:
> *"Here's our design concept (previous Q&A). Now implement the schema based on these decisions."*

#### When to Use "Grill Me"

- ✅ Before building major features or systems
- ✅ Before architectural decisions
- ✅ When requirements are fuzzy
- ✅ When integrating multiple systems
- ❌ Not needed for trivial, isolated tasks

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

#### Example: Hybrid RAG System for Narrative Consistency

**❌ Horizontal Slicing (Fails)**

| Step | Work | Problem |
|------|------|---------|
| 1 | Write entire semantic chunking script | Completes cleanly |
| 2 | Build entire Vector DB schema | Completes cleanly |
| 3 | Write agentic reasoning LLM prompts | **FAILS** |

**Why it fails**: The semantic chunk sizes chosen in Step 1 are incompatible with the context window of the reasoning agent in Step 3. Total rewrite of Step 1 required.

**✅ Vertical Slicing (Succeeds)**

| Slice | Work | Result |
|-------|------|--------|
| 1 | Hardcode one paragraph → pass through basic chunker → insert/retrieve from Vector DB → output True/False consistency flag | ✅ End-to-end success |
| 2 | Expand to 3 paragraphs, refine chunking logic | ✅ Works as expected |
| 3 | Add multiple document types, complex reasoning | ✅ Builds on proven foundation |

**Why it works**: You prove exact data payload formats between chunker, DB, and agent immediately. Each slice expands logic incrementally on a known-good foundation.

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

**❌ Horizontal Slicing (Hides Integration Risks)**

| Phase | Work | Status |
|-------|------|--------|
| 1 | Build extraction support for PDF, Image, DOCX, XLSX | ✅ Complete |
| 2 | Integrate pluggable PII detectors | ✅ Complete |
| 3 | Build masking/redaction engine | ⚠️ Problem emerges |
| 4 | Add batch processing | ❌ Blocked |

**The hidden risk**: To redact PII, you need to know *where* the detected text came from in the original document. PDF and OCR libraries may provide page numbers and bounding boxes, but some DOCX extraction libraries only return plain text without location metadata.

Result: The system can successfully detect PII, but cannot reliably mask it. The issue isn't in the masking layer—it's an extraction-layer design decision discovered too late. Fixing it may require redesigning extraction contracts or replacing extraction libraries entirely.

**✅ Vertical Slicing (Discovers Architectural Requirements Early)**

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

## Technique 4: Test-Driven Development (TDD) – Turning Requirements into Executable Constraints

### The Problem: AI Optimizes for Passing, Not Necessarily Correctness

Suppose you tell an AI:

> Build a PII masking service.

The requirement sounds simple, but it leaves many questions unanswered:

* What counts as PII?
* Should emails be masked?
* Should phone numbers be masked?
* What should the replacement format look like?
* What happens if no PII exists?
* How should PDFs, CSVs, and images be handled?

Without clear constraints, the AI fills in the blanks with assumptions. The result may look reasonable but still violate the actual requirements.

This is where TDD helps.

---

#### What TDD Actually Does

Many people think TDD is about writing tests before code.

The real value is deeper:

> TDD forces you to define expected behavior before implementation exists.

Instead of:

```text
Requirement
    ↓
AI guesses behavior
    ↓
Code
    ↓
Tests written to match code
```

You get:

```text
Requirement
    ↓
Tests define behavior
    ↓
AI must satisfy tests
    ↓
Implementation
```

The tests become an executable specification.

---

##### Phase 1: Red (Define the Behavior)

Before any implementation exists, define the expected outcome.

```python
def test_email_is_masked():
    text = "Contact me at john@example.com"

    result = mask_pii(text)

    assert result == "Contact me at [EMAIL]"
```

This test establishes several requirements:

* Emails are considered PII
* Emails must be masked
* The replacement format is `[EMAIL]`
* The surrounding text must remain unchanged

At this point the function doesn't exist.

The test fails.

This is the **Red** phase.

---

##### Phase 2: Green (Make the Test Pass)

Now instruct the AI:

> Implement the smallest amount of code needed to satisfy the test.

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

The test now passes.

This is the **Green** phase.

The implementation is no longer based on the AI's assumptions. It is constrained by the behavior defined in the test.

---

##### Phase 3: Refactor (Improve Without Changing Behavior)

Later, you may decide to:

* improve performance
* use a dedicated PII library
* switch to a machine learning model
* add caching

Example:

```python
def mask_pii(text):
    entities = pii_detector.detect(text)

    for entity in entities:
        text = text.replace(entity.value, entity.label)

    return text
```

As long as the original tests still pass, the behavior remains correct.

This is the **Refactor** phase.

The tests act as guardrails that prevent improvements from accidentally changing functionality.

---

#### Common Question: How Do I Know What Tests to Write?

The answer is:

> Write tests for behavior, not implementation.

Good tests usually fall into three categories.

##### 1. Business Rules

What must happen?

```python
def test_email_is_masked():
    ...

def test_phone_number_is_masked():
    ...
```

These represent requirements.

---

##### 2. Edge Cases

What commonly breaks?

```python
def test_empty_string():
    ...

def test_no_pii_present():
    ...

def test_multiple_emails():
    ...
```

These represent robustness requirements.

---

##### 3. Invariants

Things that should always remain true.

```python
def test_non_pii_text_is_preserved():
    ...

def test_document_line_count_is_unchanged():
    ...
```

These are often more valuable than exact output checks because they capture fundamental system behavior.

---

#### Common Question: Can the AI Cheat?

Yes.

Suppose the only test is:

```python
def test_email_masking():
    assert mask_pii(
        "Contact me at john@example.com"
    ) == "Contact me at [EMAIL]"
```

The AI could write:

```python
def mask_pii(text):
    return "Contact me at [EMAIL]"
```

The test passes.

The implementation is obviously wrong.

This is called **overfitting to the test suite**.

The AI optimized for the constraint you gave it.

The constraint was simply too weak.

---

#### How Do You Prevent Hardcoded Solutions?

##### Add More Examples

Instead of one test:

```python
def test_email_1():
    ...

def test_email_2():
    ...

def test_email_3():
    ...
```

Now hardcoding becomes more difficult.

---

##### Test Different Inputs

```python
def test_various_email_formats():
    ...

def test_multiple_emails():
    ...

def test_email_inside_sentence():
    ...
```

The AI must now handle a broader range of cases.

---

##### Test Properties Instead of Examples

Example tests are useful:

```python
assert mask_pii(
    "john@example.com"
) == "[EMAIL]"
```

But property tests are stronger:

```python
assert detected_email not in result
```

or

```python
assert len(result) > 0
assert "john@example.com" not in result
```

These validate general behavior rather than one specific example.

---

#### At What Level Should TDD Tests Exist?

Many teams focus too heavily on unit tests.

For AI-generated code, higher-level tests are often more valuable.

##### Unit Tests

Verify small pieces of logic.

```python
test_email_detection()
test_phone_detection()
```

Good for utility functions.

---

##### Integration Tests

Verify components working together.

```python
test_pdf_extraction_then_masking()
```

Checks that the pipeline functions correctly.

---

##### End-to-End Tests

Verify behavior from the user's perspective.

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

These tests are often the hardest for AI to "cheat" because they validate complete workflows rather than isolated functions.

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

The first proves something meaningful.

The second proves almost nothing.

---

#### The Biggest Benefit of TDD for AI Coding

TDD does not magically guarantee correctness.

A bad test suite produces bad software.

What TDD does provide is a mechanism for converting vague requirements into concrete, executable constraints.

Without TDD:

```text
Requirement
    ↓
AI interprets requirement
    ↓
Code
```

With TDD:

```text
Requirement
    ↓
Tests encode expected behavior
    ↓
AI writes code to satisfy behavior
    ↓
Refactoring remains safe
```

The tests become a contract between the requirement and the implementation.

That contract is especially valuable when working with AI because every missing requirement is an opportunity for the model to make assumptions. TDD systematically removes those assumptions by making the expected behavior explicit before any code is written.


#### Anti-Pattern: Horizontal Slicing in TDD

##### The Problem Explained

**Horizontal slicing** means writing all tests first, then all implementations. This violates TDD's core principle: each test should drive one small, concrete implementation. When done horizontally:

1. **You test imagined behavior, not actual behavior** – You're writing tests for features you haven't built yet, so you're guessing at what matters
2. **Tests become implementation-detail coupled** – Without knowing how the code will actually work, your tests often verify internal structure rather than external behavior
3. **Tests become brittle and useless** – They pass when behavior breaks (because they're testing the wrong thing) and fail when you refactor (because they're too tightly coupled)
4. **You lose feedback loops** – TDD works because each test teaches you something about the implementation. Writing 5 tests at once loses that learning

##### Example: Building a User Authentication System

**❌ Bad: Horizontal Slicing (All Tests First, Then All Code)**

```python
# PHASE 1: Write all tests at once (imagined behavior)
def test_user_creation():
    user = create_user("alice@example.com", "password123")
    assert user.email == "alice@example.com"
    assert user.id is not None

def test_user_validation():
    with pytest.raises(ValueError):
        create_user("", "password123")  # Empty email

def test_password_hashing():
    user = create_user("bob@example.com", "password123")
    assert user.password != "password123"  # Should be hashed
    assert verify_password(user.password, "password123")  # Can verify

def test_duplicate_user_prevention():
    create_user("alice@example.com", "pass1")
    with pytest.raises(IntegrityError):
        create_user("alice@example.com", "pass2")  # Duplicate

def test_user_retrieval():
    user = create_user("charlie@example.com", "secret")
    retrieved = get_user_by_email("charlie@example.com")
    assert retrieved.id == user.id
```

**Now implement all at once:**

```python
# PHASE 2: Implement all at once
from werkzeug.security import generate_password_hash, check_password_hash
import uuid

# You write this entire implementation without feedback
def create_user(email, password):
    if not email:
        raise ValueError("Email required")
    if len(password) < 8:
        raise ValueError("Password too short")
    
    # But wait... you never tested minimum password length!
    # Your test doesn't check this, so now you've added logic the test doesn't verify
    
    hashed = generate_password_hash(password)
    user_id = str(uuid.uuid4())
    # ... database logic ...
    return User(id=user_id, email=email, password=hashed)

def verify_password(hashed, plain):
    return check_password_hash(hashed, plain)

def get_user_by_email(email):
    # ... database lookup ...
    pass
```

**Problems that emerge:**

1. **Your tests are now too loose** – The test `test_password_hashing` just verifies the result of `verify_password()` without actually testing edge cases. What if someone passes `None`? The test doesn't care.

2. **The implementation is overspecified** – You added password length validation (`len(password) < 8`) that the tests don't check. Now your tests are insensitive to this logic—you could remove it and tests still pass.

3. **Integration bugs lurk** – When you implement `create_user`, `verify_password`, and `get_user_by_email` all at once, you might not realize they need to share a database session, or that password hashing needs to use the same library everywhere.

4. **Refactoring breaks tests** – If you later change password hashing to use a different algorithm, your tests still pass (because they're just calling `verify_password`), but suddenly the old passwords don't work. Your tests caught nothing.

##### ✅ Good: Vertical Slicing (One Test → One Implementation → Repeat)

**Cycle 1: Create and retrieve a user**

```python
# RED: Write one specific test
def test_create_and_retrieve_user():
    """A user created should be retrievable by email."""
    user = create_user("alice@example.com", "password123")
    retrieved = get_user_by_email("alice@example.com")
    assert retrieved.email == "alice@example.com"
    assert retrieved.id == user.id

# GREEN: Implement the minimal code to pass
class User:
    def __init__(self, id, email, password):
        self.id = id
        self.email = email
        self.password = password

users_db = {}

def create_user(email, password):
    user_id = len(users_db) + 1
    user = User(user_id, email, password)
    users_db[email] = user
    return user

def get_user_by_email(email):
    return users_db.get(email)

# Test PASSES. You've proven end-to-end creation and retrieval work.
```

**Cycle 2: Password hashing**

```python
# RED: Write test for password hashing
def test_password_is_hashed():
    """Passwords should not be stored in plain text."""
    user = create_user("bob@example.com", "mysecret")
    assert user.password != "mysecret"  # Not plain text

# GREEN: Implement minimal password hashing
from werkzeug.security import generate_password_hash

def create_user(email, password):
    user_id = len(users_db) + 1
    hashed_password = generate_password_hash(password)  # NEW
    user = User(user_id, email, hashed_password)
    users_db[email] = user
    return user

# Test PASSES. Password is hashed.
```

**Cycle 3: Password verification**

```python
# RED: Write test for verifying passwords
def test_verify_password():
    """Should be able to verify a password against hashed version."""
    user = create_user("charlie@example.com", "correct_password")
    assert verify_password(user.password, "correct_password") == True
    assert verify_password(user.password, "wrong_password") == False

# GREEN: Implement password verification
from werkzeug.security import check_password_hash

def verify_password(hashed, plain):
    return check_password_hash(hashed, plain)

# Test PASSES. Verification works correctly.
```

**Cycle 4: Email validation**

```python
# RED: Write test for email validation
def test_email_validation():
    """Empty email should raise an error."""
    with pytest.raises(ValueError):
        create_user("", "password123")

# GREEN: Add validation
def create_user(email, password):
    if not email:  # NEW
        raise ValueError("Email cannot be empty")
    
    user_id = len(users_db) + 1
    hashed_password = generate_password_hash(password)
    user = User(user_id, email, hashed_password)
    users_db[email] = user
    return user

# Test PASSES. Validation works.
```

##### Key Differences in Vertical Approach

- **Each cycle teaches you something** – After cycle 1, you know creation and retrieval work. After cycle 2, you know the hashing library works. Each test is concrete.
- **Tests are behavior-focused** – `test_password_is_hashed` doesn't care *how* you hash. You could switch algorithms and the test still passes (as long as hashing still works).
- **Implementation is exactly right** – You don't add extra logic (like password length validation) until you write a test for it.
- **Failures are isolated** – If password hashing breaks, only cycle 2 fails. You know exactly what's wrong.
- **Refactoring is safe** – When you refactor, you know each piece works because each was proven with a test.

#### Checklist Per TDD Cycle

- [ ] Test describes behavior, not implementation
- [ ] Test uses public interface only
- [ ] Test would survive internal refactor
- [ ] Code is minimal for this test
- [ ] No speculative features added
- [ ] Test actually fails before implementation

#### Why This Matters for AI

**TDD turns the AI from a hallucination machine into a constraint-satisfier.** The test is immutable reality. The AI must write code that satisfies it. This is profound because:

- Mathematical errors are caught immediately
- Edge cases in the test are respected
- The AI can't invent requirements
- Refactoring is safe 

### Technique 5: Deep vs. Shallow Modules – Architectural Design

#### The Problem: AI Struggles with Shallow Modules

A **shallow module** has:
- Complex, large interface
- Little internal implementation
- Dependencies scattered everywhere

When an AI is given a shallow module, it must:
- Mock five different services just to test one function
- Navigate tangled dependencies
- Understand how each service integrates
- Likely fail

**Shallow modules are AI nightmares.**

#### The Solution: Design Deep Modules

A **deep module** has:
- Small, simple interface
- Large, complex implementation hidden inside
- Few external dependencies
- Clear contract with callers

**Deep modules are AI dreams.**

#### Visual Comparison

```
┌─────────────────────┐         ┌─────────────────────────────────┐
│   Small Interface   │         │       Large Interface           │
│  (few methods)      │         │  (many methods, complex params) │
├─────────────────────┤         ├─────────────────────────────────┤
│                     │         │                                 │
│                     │         │  Thin Implementation           │
│  Deep               │         │  (just passes through)         │
│  Implementation     │         │                                 │
│                     │         │                                 │
│                     │         │                                 │
│                     │         │                                 │
└─────────────────────┘         └─────────────────────────────────┘

    GOOD                             BAD (Avoid)
   (AI-friendly)                  (AI-hostile)
```

#### Example: Multi-Agent Financial Research Platform

**❌ Shallow Architecture (AI Nightmare)**

```python
# AI must orchestrate all of these manually
def fetch_sec_filings(ticker): ...
def run_bayesian_inference(data, prior): ...
def calculate_kl_divergence(dist_p, dist_q): ...
def generate_market_signal(): ...

# To test generate_market_signal, AI must:
# 1. Mock fetch_sec_filings
# 2. Mock run_bayesian_inference
# 3. Mock calculate_kl_divergence
# 4. Wire them all together correctly
# 5. Verify the final output
# This complexity explodes quickly.
```

**✅ Deep Architecture (AI Dream)**

```python
class MarketSignalEngine:
    """Simple interface hides complex logic."""
    
    def evaluate_ticker(self, ticker: str) -> SignalResult:
        """Evaluate a ticker and return a signal."""
        pass

# AI implements:
# - SEC filing fetching
# - Statistical inference
# - Signal generation
# All hidden behind one simple method.

# To test: Just call evaluate_ticker and verify the result.
# All the complex orchestration is internal.
```

#### Design Principles for Deep Modules

When designing interfaces, ask:

1. **Can I reduce the number of methods?**
   - Instead of: `get_data()`, `process()`, `validate()`, `store()`
   - Try: `execute(raw_input)` → processes and returns result

2. **Can I simplify the parameters?**
   - Instead of: `process(data, config, cache, logger, error_handler)`
   - Try: `process(data, config)` → logger and cache are internal

3. **Can I hide more complexity inside?**
   - Instead of: Caller handles error retry logic, logging, caching
   - Try: Module handles all of it internally, returns clean result

#### Why You Design the Interface, AI Implements Inside

**Principle**: You design architecture, AI does the work.

```
Your job:                          AI's job:
- Define interface                 - Implement logic
- Choose module boundaries         - Handle edge cases
- Set constraints                  - Optimize performance
- Specify contracts                - Write tests
```

This division of labor is crucial. AI is excellent at **implementation complexity**. Humans are excellent at **architectural judgment**.

#### Interface Design for Testability

Good interfaces for testing:

##### 1. Accept dependencies, don't create them

```python
# ✅ Testable
def process_order(order, payment_gateway):
    return payment_gateway.charge(order.total)

# ❌ Hard to test
def process_order(order):
    gateway = StripeGateway()  # Can't mock this
    return gateway.charge(order.total)
```

Why it matters: In the testable version, you pass `payment_gateway` as a parameter. In tests, you can pass a mock:

```python
class MockPaymentGateway:
    def charge(self, amount):
        return {"success": True, "amount": amount}

# Test
order = Order(total=100)
result = process_order(order, MockPaymentGateway())
assert result["success"] == True
```

In the non-testable version, `process_order` creates `StripeGateway()` internally, so you can't replace it with a mock.

##### 2. Return results, don't produce side effects

```python
# ✅ Testable
def calculate_discount(cart):
    """Returns a discount object without modifying cart."""
    return {"amount": 10, "reason": "loyalty"}

# ❌ Hard to test
def apply_discount(cart):
    """Modifies cart in place (side effect)."""
    cart["total"] -= 10  # Can't verify this easily
    return None
```

Why it matters: With the testable version, you can easily verify the return value:

```python
# Test
cart = {"items": [item1, item2], "total": 100}
discount = calculate_discount(cart)
assert discount["amount"] == 10
assert cart["total"] == 100  # Original unchanged
```

With the non-testable version, you have to check if `cart` was modified:

```python
# Test (awkward)
cart = {"items": [item1, item2], "total": 100}
apply_discount(cart)  # Returns None, so what do we test?
assert cart["total"] == 90  # Had to check internal state
```

##### 3. Minimize surface area

- Fewer methods = fewer tests needed
- Fewer parameters = simpler test setup
- Clearer contracts = fewer surprises

Example:

```python
# ❌ Large surface area (many methods to test)
class OrderProcessor:
    def validate_order(self, order): pass
    def check_inventory(self, order): pass
    def apply_discounts(self, order): pass
    def process_payment(self, order): pass
    def create_shipment(self, order): pass
    def send_confirmation(self, order): pass
    # 6 methods = 6+ tests minimum

# ✅ Small surface area (one method, hides complexity)
class OrderProcessor:
    def process(self, order, payment_gateway, inventory, mailer):
        # All logic hidden inside
        pass
    # 1 method = 1 core test
```

#### Checklist: Deep Module Design

- [ ] Interface has ≤ 3 primary methods
- [ ] Each method has ≤ 3 required parameters
- [ ] Dependencies are injected (not created internally)
- [ ] Returns results (minimal side effects)
- [ ] Internal complexity is substantial
- [ ] Contract is clear and testable

---

## Part 4: Key Takeaways & Operational Habits

1. **Optimize context, not prompts**: The best prompts mean nothing without a structured Second Brain (`INDEX.md`, `CURRENT_WORK.md`).
2. **Integrate continuously via Vertical Slicer**: Prove one tracer bullet path across the entire stack before expanding laterally. 
3. **Tests are contracts**: Use vertical TDD to constrain the AI tightly into mathematical or business reality.
4. **Humans own architecture**: Design Deep Modules with simple interfaces so the AI doesn't drown in orchestrations.
5. **Always Clear Context**: Stop compacting chats. Keep the AI in the "Smart Zone" by resetting sessions between disjoint tasks.

**The shift**: Move from *"Let AI build this feature, I'll review it"* to *"I'll design the interface and test, AI will implement, and the Second Brain will retain the context."*
