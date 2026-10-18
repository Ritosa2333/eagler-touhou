#include "Chain.hpp"

#include <stddef.h>

Chain g_Chain;

Chain::~Chain()
{
}

ChainElem::ChainElem()
{
    this->prev = NULL;
    this->next = NULL;
    this->callback = NULL;
    this->unkPtr = this;
    this->addedCallback = NULL;
    this->deletedCallback = NULL;
    this->priority = 0;
    this->isAllocated = 0;
}

ChainElem::~ChainElem()
{
    if (this->deletedCallback)
    {
        (*this->deletedCallback)(this->arg);
    }
    this->prev = NULL;
    this->next = NULL;
    this->callback = NULL;
    this->addedCallback = NULL;
    this->deletedCallback = NULL;
}

Chain::Chain()
{
}

ZunResult Chain::AddToCalcChain(ChainElem *elem, i32 priority)
{
    ZunResult uVar1;
    ChainElem *curElem;

    curElem = &this->calcChain;
    elem->priority = priority;
    while (curElem->next)
    {
        if (curElem->priority > priority)
        {
            break;
        }
        curElem = curElem->next;
    }
    if (curElem->priority > priority)
    {
        elem->next = curElem;
        elem->prev = curElem->prev;
        if (elem->prev)
        {
            elem->prev->next = elem;
        }
        curElem->prev = elem;
    }
    else
    {
        elem->next = NULL;
        elem->prev = curElem;
        curElem->next = elem;
    }
    if (elem->addedCallback)
    {
        uVar1 = elem->addedCallback(elem->arg);
        elem->addedCallback = NULL;
        return uVar1;
    }
    else
    {
        return ZUN_SUCCESS;
    }
}

ZunResult Chain::AddToDrawChain(ChainElem *elem, i32 priority)
{
    ChainElem *curElem;

    curElem = &this->drawChain;
    elem->priority = priority;
    while (curElem->next)
    {
        if (curElem->priority > priority)
        {
            break;
        }
        curElem = curElem->next;
    }
    if (curElem->priority > priority)
    {
        elem->next = curElem;
        elem->prev = curElem->prev;
        if (elem->prev)
        {
            elem->prev->next = elem;
        }
        curElem->prev = elem;
    }
    else
    {
        elem->next = NULL;
        elem->prev = curElem;
        curElem->next = elem;
    }
    if (elem->addedCallback)
    {
        return elem->addedCallback(elem->arg);
    }
    else
    {
        return ZUN_SUCCESS;
    }
}

i32 Chain::RunCalcChain()
{
    ChainElem *next;
    ChainElem *current;
    i32 updateCount;

restart_from_first_job:
    updateCount = 0;
    current = &this->calcChain;
    while (current)
    {
        if (current->callback)
        {
        execute_again:
            switch (current->callback(current->arg))
            {
            case CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB:
                next = current;
                current = current->next;
                Cut(next);
                updateCount++;
                continue;
            case CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN:
                goto execute_again;
            case CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS:
                return 0;
            case CHAIN_CALLBACK_RESULT_BREAK:
                return 1;
            case CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR:
                return -1;
            case CHAIN_CALLBACK_RESULT_RESTART_FROM_FIRST_JOB:
                goto restart_from_first_job;
            default:
                break;
            }
            updateCount++;
        }
        current = current->next;
    }
    return updateCount;
}

i32 Chain::RunDrawChain()
{
    ChainElem *next;
    ChainElem *current;
    i32 updateCount;

    updateCount = 0;
    current = &this->drawChain;
    while (current)
    {
        if (current->callback)
        {
        execute_again:
            switch (current->callback(current->arg))
            {
            case CHAIN_CALLBACK_RESULT_CONTINUE_AND_REMOVE_JOB:
                next = current;
                current = current->next;
                Cut(next);
                updateCount++;
                continue;
            case CHAIN_CALLBACK_RESULT_EXECUTE_AGAIN:
                goto execute_again;
            case CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS:
                return 0;
            case CHAIN_CALLBACK_RESULT_BREAK:
                return 1;
            case CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR:
                return -1;
            default:
                break;
            }
            updateCount++;
        }
        current = current->next;
    }
    return updateCount;
}

void Chain::ReleaseSingleChain(ChainElem *root)
{
    ChainElem nextRootElem;
    ChainElem *tmp2;
    ChainElem *tmp;
    ChainElem *curElem;

    tmp = new ChainElem;
    nextRootElem.next = tmp;
    curElem = root;
    while (curElem)
    {
        tmp->unkPtr = curElem;
        tmp->next = new ChainElem;
        tmp = tmp->next;
        curElem = curElem->next;
    }
    curElem = &nextRootElem;
    while (curElem)
    {
        Cut(curElem->unkPtr);
        curElem = curElem->next;
    }
    tmp = nextRootElem.next;
    while (tmp)
    {
        tmp2 = tmp->next;
        delete tmp;
        tmp = NULL;
        tmp = tmp2;
    }
}

void Chain::Release()
{
    ReleaseSingleChain(&this->calcChain);
    ReleaseSingleChain(&this->drawChain);
}

ChainElem *Chain::CreateElem(ChainCallback callback)
{
    ChainElem *elem = new ChainElem;
    elem->callback = callback;
    elem->addedCallback = NULL;
    elem->deletedCallback = NULL;
    elem->isAllocated = 1;
    return elem;
}

void Chain::Cut(ChainElem *toRemove)
{
    ChainElem *curElem;

    if (!toRemove)
    {
        return;
    }

    curElem = &this->calcChain;
    while (curElem)
    {
        if (curElem == toRemove)
        {
            goto destroy_elem;
        }
        curElem = curElem->next;
    }
    curElem = &this->drawChain;
    while (curElem)
    {
        if (curElem == toRemove)
        {
            goto destroy_elem;
        }
        curElem = curElem->next;
    }

    return;

destroy_elem:
    if (toRemove->prev)
    {
        toRemove->callback = NULL;
        toRemove->prev->next = toRemove->next;
        if (toRemove->next)
        {
            toRemove->next->prev = toRemove->prev;
        }
        toRemove->prev = NULL;
        toRemove->next = NULL;

        if (toRemove->isAllocated)
        {
            delete toRemove;
            toRemove = NULL;
        }
        else
        {
            if (toRemove->deletedCallback)
            {
                ChainLifecycleCallback deletedCallback = toRemove->deletedCallback;
                toRemove->deletedCallback = NULL;
                deletedCallback(toRemove->arg);
            }
        }
    }
}
