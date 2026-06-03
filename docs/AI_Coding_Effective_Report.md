# Effective AI Coding: A Comprehensive Guide to Working with AI Agents

**Presentation Report | Duration: 30 minutes**

---

## Table of Contents

1. [Introduction & Core Philosophy](#introduction--core-philosophy)
2. [Technique 1: Context Management – Surviving the "Dumb Zone"](#technique-1-context-management--surviving-the-dumb-zone)
3. [Technique 2: The "Grill Me" Skill – Pre-Computation Alignment](#technique-2-the-grill-me-skill--pre-computation-alignment)
4. [Technique 3: Vertical Slicing – Tracer Bullets](#technique-3-vertical-slicing--tracer-bullets)
5. [Technique 4: Test-Driven Development (TDD) – The Ultimate AI Constraint](#technique-4-test-driven-development-tdd--the-ultimate-ai-constraint)
6. [Technique 5: Deep vs. Shallow Modules – Architectural Design](#technique-5-deep-vs-shallow-modules--architectural-design)
7. [Technique 6: Kanban DAG – Parallelization at Scale](#technique-6-kanban-dag--parallelization-at-scale)
8. [Key Takeaways & Implementation Strategy](#key-takeaways--implementation-strategy)

---

## Introduction & Core Philosophy

### The Problem: "Specs to Code" Is Not Enough

When developers first began working with AI coding agents, the expectation was simple:
- Write requirements
- AI builds the feature
- Ship it

**Reality**: This approach fails repeatedly. AI agents produce mediocre, tangled code without proper guidance. Why? Because AI doesn't understand architecture, taste, or long-term codebase health.

### The Thesis: Software Engineering Fundamentals Still Apply

The key insight from this workshop is that **good software engineering fundamentals don't become obsolete with AI—they become mandatory**. AI works best when given:

- Small, clear tasks
- Strong tests as constraints
- Good module boundaries
- Simple interfaces
- Deep, well-structured modules (not shallow, tangled files)
- Human review and quality assurance

### The New Role of Humans

In AI-assisted development:
- **Humans own**: Planning, architecture, QA, and taste
- **AI owns**: Implementation of well-scoped tasks
- **Collaboration**: Tests, code review, and feedback loops keep AI on track

---

## Technique 1: Context Management – Surviving the "Dumb Zone"

### What Is the "Dumb Zone"?

LLM attention mechanisms suffer from **quadratic performance degradation** with context length. Feed an LLM massive context, and it will:
- Silently drop constraints
- Hallucinate variables and dependencies
- Lose track of the core objective
- Generate generic, wrong solutions

This happens *silently*—the AI will confidently output plausible-but-incorrect code.

### Why It Matters

Every line of old code, terminal output, chat history, and tangential information adds "token pollution." The AI's focus deteriorates in proportion to context size.

### The Solution: The "Smart Zone"

**Strategy**: Clear context entirely between tasks. Start fresh with an isolated system prompt and only essential information.

### Example: Python Topological Sort Debugging

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

**Example of what goes wrong:**

```python
# WHAT AI SEES (confusing mix of old and new):
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
    assert result is None or result == []  # Should detect cycle, not infinite loop
```

**AI's confused output:**
- Can't tell which version is current
- Doesn't know cycle detection is required
- Outputs generic DFS that will infinite loop on cycles
```

---

**✅ Winning Strategy (Smart Zone)**
```
Context:
- ONLY the topological_sort.py file
- ONLY the one failing test
- Precise prompt:
```

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
```

### Implementation Checklist

- [ ] Clear chat context entirely between major tasks
- [ ] Provide only the minimal, isolated file(s) needed
- [ ] Write precise, specific prompts (not vague requests)
- [ ] Include the exact expected output or constraint
- [ ] For debugging: provide only the broken file, not the whole codebase
- [ ] For new features: provide only the interface, not implementation details

### Key Insight

**Your job is filtering, not copying.** Curate what the AI sees. The quality of AI output is proportional to the signal-to-noise ratio of the context you provide.

---

## Technique 2: The "Grill Me" Skill – Pre-Computation Alignment

### The Problem: Vague Prompts → Generic Architecture

When you ask an AI agent to "build a feature," it will:
- Eagerly agree with your vague requirements
- Generate plausible-but-generic architecture
- Miss critical edge cases and constraints
- Require total rewrites when reality emerges

The root cause: **ambiguity about what "done" actually means**.

### Why This Technique Works

The "Grill Me" prompt forces the AI to interrogate you relentlessly *before* writing any code. This pre-computation alignment:
- Maps out edge cases
- Resolves design tree branches
- Clarifies constraints
- Produces a clear mental model both you and the AI share

### The Workflow

Instead of asking: *"Create a database schema for a video production supply chain"*

Ask: *"Interview me relentlessly about the database schema for a video production supply chain. Walk down the design tree resolving dependencies. Ask one question at a time and provide recommendations."*

### Example: Video Production Supply Chain

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

### Output: The Design Concept (Not a PRD)

The Q&A history itself becomes your **immutable "Design Concept"**. You don't need a formal PRD document—the conversation IS the requirements.

Use this design concept when prompting the actual coding agent:
> *"Here's our design concept (previous Q&A). Now implement the schema based on these decisions."*

### When to Use "Grill Me"

- ✅ Before building major features or systems
- ✅ Before architectural decisions
- ✅ When requirements are fuzzy
- ✅ When integrating multiple systems
- ❌ Not needed for trivial, isolated tasks

---

## Technique 3: Vertical Slicing – Tracer Bullets

### The Problem: Horizontal Implementation Fails

AI naturally codes "horizontally" (layer by layer):
1. Build entire database schema
2. Build entire API layer
3. Build entire UI layer

This approach **delays integration testing** and almost guarantees failure in complex systems. By the time layers connect, incompatibilities are discovered too late.

### Why Vertical Slicing Works

A **tracer bullet** is a hyper-narrow slice of functionality that cuts through **every layer** of the stack immediately. It:
- Proves the end-to-end path works
- Reveals format/size mismatches early
- Is demoable/testable on its own
- Builds momentum (visible progress)

### Example: Hybrid RAG System for Narrative Consistency

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

### Implementation Pattern

For any feature:

1. **Identify layers**: UI → API → Business Logic → Data
2. **Pick the simplest path**: Hardcode data, minimal logic
3. **Build one complete slice**: Walk through every layer end-to-end
4. **Verify and test**: Confirm integration works
5. **Expand incrementally**: Add complexity one slice at a time

### Why AI Loves Vertical Slices

- Small scope = stays in "Smart Zone" (low token pollution)
- Concrete, testable milestones = clear acceptance criteria
- Failures are isolated to one layer = easy debugging
- Each success builds momentum

---

## Technique 4: Test-Driven Development (TDD) – The Ultimate AI Constraint

### The Problem: "Code Then Test" Produces Brittle Code

If you tell an AI: *"Write code, then write tests,"* it will:
- Write implementation first (often hallucinated or generic)
- Write tests designed specifically to pass that implementation
- Tests become coupled to implementation details
- Tests break when you refactor, even though behavior is unchanged

**Tests become useless as constraints.**

### Why TDD Works: Tests as Guardrails

By enforcing **TDD (Test-First)**, the AI is constrained by mathematical or logical reality:
- Red phase: Test fails, proving it's testing real behavior
- Green phase: AI writes code specifically to satisfy the constraint
- Refactor phase: Tests prove the core logic survives changes

**Tests become immutable specifications, not implementation verification.**

### The Three Phases

#### Phase 1: Red (Failing Test)

Instruct the AI: *"Write a test suite first. Make it fail."*

**Example: SIR Epidemic Model**

```python
def test_infection_rate_calculation():
    """Test SIR model infection rate calculation."""
    S = 100  # Susceptible
    I = 5    # Infected
    beta = 0.1  # Transmission rate
    
    result = calculate_infection_rate(S, I, beta)
    
    # Mathematical constraint: beta * S * I
    # 0.1 * 100 * 5 = 50
    assert result == 50, f"Expected 50, got {result}"

# Run test → FAILS (function is empty)
```

The test encodes a **mathematical truth**, not an implementation detail.

#### Phase 2: Green (Minimal Implementation)

Instruct: *"Now implement calculate_infection_rate to make the test pass."*

```python
def calculate_infection_rate(S: int, I: int, beta: float) -> float:
    """
    Calculate infection rate using SIR model.
    
    I' = beta * S * I
    """
    return beta * S * I

# Run test → PASSES
```

The implementation is **forced** to respect the mathematical constraint.

#### Phase 3: Refactor (Optimization)

Instruct: *"Optimize this using vectorized NumPy operations. Keep the test passing."*

```python
import numpy as np

def calculate_infection_rate(S: np.ndarray, I: np.ndarray, 
                             beta: float) -> np.ndarray:
    """Vectorized SIR infection rate calculation."""
    return beta * S * I

# Run test → STILL PASSES (refactored but behavior identical)
```

The test proves optimization didn't break logic.

### Anti-Pattern: Horizontal Slicing in TDD

#### The Problem Explained

**Horizontal slicing** means writing all tests first, then all implementations. This violates TDD's core principle: each test should drive one small, concrete implementation. When done horizontally:

1. **You test imagined behavior, not actual behavior** – You're writing tests for features you haven't built yet, so you're guessing at what matters
2. **Tests become implementation-detail coupled** – Without knowing how the code will actually work, your tests often verify internal structure rather than external behavior
3. **Tests become brittle and useless** – They pass when behavior breaks (because they're testing the wrong thing) and fail when you refactor (because they're too tightly coupled)
4. **You lose feedback loops** – TDD works because each test teaches you something about the implementation. Writing 5 tests at once loses that learning

#### Example: Building a User Authentication System

❌ **Bad: Horizontal Slicing (All Tests First, Then All Code)**

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

---

✅ **Good: Vertical Slicing (One Test → One Implementation → Repeat)**

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

**Key differences in the vertical approach:**

- **Each cycle teaches you something** – After cycle 1, you know creation and retrieval work. After cycle 2, you know the hashing library works. Each test is concrete.
- **Tests are behavior-focused** – `test_password_is_hashed` doesn't care *how* you hash. You could switch algorithms and the test still passes (as long as hashing still works).
- **Implementation is exactly right** – You don't add extra logic (like password length validation) until you write a test for it.
- **Failures are isolated** – If password hashing breaks, only cycle 2 fails. You know exactly what's wrong.
- **Refactoring is safe** – When you refactor, you know each piece works because each was proven with a test.

#### Why Horizontal Slicing Fails with AI

When you give an AI 5 tests to implement at once:

1. **It hallucinate edge cases** – "What if email has a @? What if password is null?" – The test doesn't check, so AI might add logic that's wrong.
2. **It over-engineers** – "I'll make this production-ready" – You get 300 lines when 30 would pass the tests.
3. **Tests become decoupled from reality** – Your tests pass but the actual system fails because edge cases weren't exercised.
4. **Debugging is a nightmare** – 5 failures at once. Is it the hashing? The validation? The database? Good luck.

#### Checklist: Vertical vs. Horizontal

| Aspect | Vertical (Good) | Horizontal (Bad) |
|--------|-----------------|------------------|
| **Test count per cycle** | 1 test | 5+ tests |
| **Implementation size** | Minimal | Maximal |
| **Test coupling** | Behavior-focused | Implementation-coupled |
| **Failure isolation** | Single cycle fails | Multiple cycles unclear |
| **Feedback loop** | Tight (test → learn → next test) | Loose (tests → guessing → code) |
| **Refactoring safety** | High (each test validates piece) | Low (tests might not catch changes) |
| **AI success rate** | High (clear constraint) | Low (ambiguous what to implement) |

### Checklist Per TDD Cycle

- [ ] Test describes behavior, not implementation
- [ ] Test uses public interface only
- [ ] Test would survive internal refactor
- [ ] Code is minimal for this test
- [ ] No speculative features added
- [ ] Test actually fails before implementation

### Why This Matters for AI

**TDD turns the AI from a hallucination machine into a constraint-satisfier.** The test is immutable reality. The AI must write code that satisfies it. This is profound because:

- Mathematical errors are caught immediately
- Edge cases in the test are respected
- The AI can't invent requirements
- Refactoring is safe

---

## Technique 5: Deep vs. Shallow Modules – Architectural Design

### The Problem: AI Struggles with Shallow Modules

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

### The Solution: Design Deep Modules

A **deep module** has:
- Small, simple interface
- Large, complex implementation hidden inside
- Few external dependencies
- Clear contract with callers

**Deep modules are AI dreams.**

### Visual Comparison

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

### Example: Multi-Agent Financial Research Platform

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

### Design Principles for Deep Modules

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

### Why You Design the Interface, AI Implements Inside

**Principle**: You design architecture, AI does the work.

```
Your job:                          AI's job:
- Define interface                 - Implement logic
- Choose module boundaries         - Handle edge cases
- Set constraints                  - Optimize performance
- Specify contracts                - Write tests
```

This division of labor is crucial. AI is excellent at **implementation complexity**. Humans are excellent at **architectural judgment**.

### Interface Design for Testability

Good interfaces for testing:

**1. Accept dependencies, don't create them**
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

**2. Return results, don't produce side effects**
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

**3. Minimize surface area**
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

---

## Technique 6: Kanban DAG – Parallelization at Scale

### The Problem: Sequential AI Execution Is Slow

Current approach:
- Write issue 1 → AI executes → Merge
- Write issue 2 → AI executes → Merge
- Write issue 3 → AI executes → Merge

**Sequential execution limits throughput to one AI agent at a time.**

### The Solution: Kanban DAG (Directed Acyclic Graph)

Convert your plan into a DAG of isolated Markdown issues. Run multiple AI agents in parallel, each in a sandboxed Git branch.

**Key insight**: Dependency graph unlocks parallelization.

### Example: E-Commerce Platform

```
Issue 1: Initialize PostgreSQL schema
├─ Issue 2: Build data ingestion API (blocked by Issue 1)
└─ Issue 3: Build webhook listeners (blocked by Issue 1)
```

**Execution Flow:**

```
Time T0: Agent A starts Issue 1 (schema)

Time T1: Issue 1 completes and merges
       ├─ Agent A picks up Issue 2 (API)
       └─ Agent B picks up Issue 3 (webhooks) SIMULTANEOUSLY

Time T2: Both Issue 2 and Issue 3 complete
       └─ Merge agent resolves any conflicts
       └─ System unified
```

**Result**: 40% faster than sequential (varies by dependency depth).

### DAG Structure: Rules

1. **Identify hard dependencies**: Which tasks must complete first?
2. **Eliminate false dependencies**: Tasks that *seem* to block but don't
3. **Create independent slices**: Each issue is a complete, testable unit
4. **Order by blockers**: Represent as DAG with blocking relationships

### Example: Full Workflow DAG

```
0. Setup & Configuration
   ├─ 1. Database Schema (blocks 2, 3, 4)
   │   ├─ 2. ORM Models (blocks 5)
   │   │   └─ 5. API Endpoints (blocks 6)
   │   ├─ 3. Migration Scripts
   │   └─ 4. Seed Data
   │
   ├─ 6. Authentication Service
   │   └─ 7. Protected Routes
   │
   └─ 8. Frontend (blocks 9)
       └─ 9. Integration Tests
```

**Reading the DAG:**
- Issues 2, 3, 4 can run in parallel (all blocked by 1)
- Issue 6 can run independently
- Issue 5 requires 2 to complete first
- Issue 7 requires 6 to complete first

### Scheduling Multiple Agents

With a DAG, you can:

```
Agent A: Executes Issue 1, then Issue 2
Agent B: Executes Issue 3, then Issue 5
Agent C: Executes Issue 4, then Issue 7
Agent D: Executes Issue 6 and Issue 8
```

Once blockers are unblocked, agents pick up next available work.

### Implementation: Markdown Issues

Each issue is a markdown file with clear acceptance criteria and blockers:

```markdown
## Issue 2: Build Data Ingestion API

### Blocked by
- Issue 1 (Database Schema must exist)

### What to build
REST API for ingesting product data. Takes JSON, validates schema, 
stores in PostgreSQL using ORM models from Issue 2.

### Acceptance Criteria
- [ ] POST /api/products accepts valid product JSON
- [ ] Returns 400 for invalid schema
- [ ] Data persists correctly
- [ ] 95%+ request success rate under 100 req/sec load

### User Stories Addressed
- Story 3: "Admin can upload product catalog"
- Story 7: "Bulk operations complete within 30 seconds"
```

### Why This Scales AI Coding

- **Parallelism**: N agents → N× throughput (ignoring merge overhead)
- **Isolation**: Each issue is self-contained, stays in "Smart Zone"
- **Debuggability**: Failures are isolated to single issues
- **Flexibility**: Agents can re-prioritize based on blockers
- **Merge safety**: Clear contract between issues makes conflicts rare

### Tools & Workflow

Modern tools (e.g., Sand Castle, GitHub Actions) support:
- Automatic branch creation per issue
- Parallel agent execution
- Merge conflict detection and resolution
- Status dashboards

---

## Key Takeaways & Implementation Strategy

### The Six Techniques At a Glance

| Technique | Problem It Solves | When to Use | Key Benefit |
|-----------|------------------|------------|------------|
| **Smart Zone** | Token pollution, hallucination | Every AI task | Crisp, correct output |
| **Grill Me** | Vague requirements, generic architecture | Before building features | Aligned understanding before coding |
| **Vertical Slicing** | Integration failures, late discovery | Organizing complex features | Early proof, fail fast |
| **TDD** | Brittle code, hallucinated logic | Writing core logic | Tests are contracts, not verification |
| **Deep Modules** | Shallow, tangled code; hard to test | Architectural design | Simple interfaces, testable boundaries |
| **Kanban DAG** | Sequential execution bottleneck | Managing large projects | Parallel execution, faster delivery |

### Implementation Roadmap

#### Phase 1: Personal Discipline (Week 1-2)
Start with **Smart Zone** and **TDD** on personal projects:
- [ ] Clear context before each AI task
- [ ] Write tests first, implement second
- [ ] Track token efficiency (smaller context = better results)

#### Phase 2: Team Process (Week 3-4)
Introduce **Grill Me** and **Vertical Slicing**:
- [ ] Use "Grill Me" skill before feature work
- [ ] Break features into tracer bullets
- [ ] Require acceptance criteria on each slice

#### Phase 3: Architecture (Week 5-6)
Design **Deep Modules**:
- [ ] Audit existing codebase for shallow modules
- [ ] Design deep module interfaces for new work
- [ ] Use AI to implement inside deep module boundaries

#### Phase 4: Scale (Week 7+)
Implement **Kanban DAG** for parallelization:
- [ ] Map projects as dependency graphs
- [ ] Create Markdown issues with clear blockers
- [ ] Run multiple agents in parallel

### Critical Success Factors

**1. Tests are contracts, not implementation verification**
- AI must satisfy tests, not just pass them
- Tests describe behavior, not implementation
- Test failure = broken contract, not broken code

**2. Humans own architecture, AI owns implementation**
- You design interfaces, AI implements them
- You decide module boundaries, AI implements deeply
- You specify constraints, AI satisfies them

**3. Stay in the "Smart Zone"**
- Minimize context for each task
- One clear prompt per task
- Clear acceptance criteria

**4. Iterate vertically, not horizontally**
- One tracer bullet at a time
- Full integration at each step
- Proof before expansion

**5. Humans review everything**
- Code review: Verify AI followed spec
- QA testing: Verify behavior matches tests
- Architecture: Verify long-term codebase health

### Anti-Patterns to Avoid

❌ **"Build the whole feature in one prompt"**
- Too much context = Dumb Zone
- No integration testing until the end
- Total rewrite if mistakes found

✅ **Build vertical slices, integrate continuously**

---

❌ **"Let AI decide the architecture"**
- AI defaults to shallow, tangled modules
- Poor testability emerges later
- Refactoring becomes massive effort

✅ **You design interfaces, AI implements inside them**

---

❌ **"Write tests after implementation"**
- Tests become coupled to implementation
- Not real constraints, just verification
- Brittle tests that break on refactors

✅ **Write tests first, implement to spec**

---

❌ **"Squeeze all code into one context window"**
- Quadratic degradation in attention
- Silent hallucinations
- Mediocre, generic output

✅ **Minimize context, maximize clarity**

---

### Measuring Success

Track these metrics to validate the approach:

| Metric | Target | Why It Matters |
|--------|--------|-----------------|
| Code review findings per PR | < 2 issues | AI stays on spec |
| Test pass rate | > 95% | Tests are real constraints |
| Module testability (tests per module) | > 80% | Deep modules are testable |
| Feature delivery cycle time | Decreasing | Parallelization works |
| Rework ratio (hours debugging / hours coding) | < 10% | Quality improves |
| AI task success rate | > 90% | Clear scoping works |

### The Mindset Shift

**Old paradigm:**
> "Let AI build this feature. I'll review it."

**New paradigm:**
> "I'll design the interface and write the test. AI will implement and stay within spec. I'll review that it satisfies the test and maintains architecture."

This shift moves you from **trust-and-hope** to **specification-driven**, which is why AI coding can actually work at scale.

---

## Conclusion

Effective AI coding isn't about prompt engineering or finding the perfect chat. It's about **applying decades of software engineering wisdom** to a new tool.

The six techniques in this report:
1. Keep AI in the "Smart Zone" (context management)
2. Force alignment before coding (Grill Me)
3. Build end-to-end incrementally (vertical slicing)
4. Constrain with tests (TDD)
5. Design testable interfaces (deep modules)
6. Parallelize execution (Kanban DAG)

These aren't new ideas. They're battle-tested practices from:
- TDD pioneers (Kent Beck, Martin Fowler)
- Architecture philosophers (John Ousterhout)
- Agile practitioners (incremental delivery)
- Systems thinkers (DAGs, parallelization)

**The insight**: These practices work even better with AI because AI needs clear constraints, explicit specs, and testable boundaries.

Your job is to provide them. AI's job is to satisfy them. Together, you build systems that are well-tested, well-architected, and maintainable.

---

## Quick Reference: 30-Minute Talk Structure

**Introduction (3 min)**
- Problem: Vague AI prompts → messy code
- Thesis: Software fundamentals + AI = better output
- Preview: Six techniques

**Technique 1: Smart Zone (4 min)**
- Why: Token pollution causes hallucination
- Example: C++ algorithm
- Action: Minimize context

**Technique 2: Grill Me (4 min)**
- Why: Vague specs are useless
- Example: Database schema Q&A
- Action: Interview before coding

**Technique 3: Vertical Slicing (3 min)**
- Why: Horizontal slicing = late failure
- Example: RAG system
- Action: Build tracer bullets

**Technique 4: TDD (4 min)**
- Why: Tests are constraints, not verification
- Example: SIR model
- Action: Red → Green → Refactor

**Technique 5: Deep Modules (4 min)**
- Why: Shallow modules = untestable
- Example: Financial platform
- Action: Design interface, AI implements

**Technique 6: Kanban DAG (3 min)**
- Why: Sequential = slow
- Example: E-commerce platform
- Action: Run agents in parallel

**Wrap-up (2 min)**
- Key takeaway: Spec-driven, not prompt-driven
- Implementation strategy: Phase 1-4
- Call to action: Start with Smart Zone + TDD

---

**Total: 31 minutes (includes 1 min buffer)**

